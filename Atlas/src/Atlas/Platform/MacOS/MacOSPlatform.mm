#include "atpch.h"
#include "MacOSPlatform.h"

#include "AtlasPaths.h"
#include "Atlas/Core/Platform.h"

#include <AppKit/AppKit.h>
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <objc/objc.h>

namespace Atlas {
Platform* Platform::s_instance = new MacOSPlatform();

std::string MacOSPlatform::getResourcesPathImpl() {
	CFBundleRef bundle		 = CFBundleGetMainBundle();
	CFURLRef	resourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);
	char		path[PATH_MAX];
	CFURLGetFileSystemRepresentation(resourcesURL, true, (UInt8*)path, PATH_MAX);
	CFRelease(resourcesURL);
	return std::string(path);
}

std::string MacOSPlatform::getExecutablePathImpl() {
	CFBundleRef bundle		  = CFBundleGetMainBundle();
	CFURLRef	executableURL = CFBundleCopyExecutableURL(bundle);
	char		path[PATH_MAX];
	CFURLGetFileSystemRepresentation(executableURL, true, (UInt8*)path, PATH_MAX);
	CFRelease(executableURL);
	return std::string(path);
}

std::string MacOSPlatform::getAppSupportPathImpl() {
	NSArray*  paths		 = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString* appSupport = [paths firstObject];
	NSString* atlasPath	 = [appSupport stringByAppendingPathComponent:@"Atlas Editor"];

	NSFileManager* fm = [NSFileManager defaultManager];
	if (![fm fileExistsAtPath:atlasPath]) {
		[fm createDirectoryAtPath:atlasPath withIntermediateDirectories:YES attributes:Nil error:Nil];
	}

	return std::string([atlasPath UTF8String]);
}

std::string MacOSPlatform::openFileDialogImpl(const std::string& filter) {
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setCanChooseFiles:YES];
	[panel setCanChooseDirectories:NO];
	[panel setAllowsMultipleSelection:NO];

	if (!filter.empty()) {
		NSString* ext  = [NSString stringWithUTF8String:filter.c_str()];
		UTType*	  type = [UTType typeWithFilenameExtension:ext];
		[panel setAllowedContentTypes:@[ type ]];
	}

	if ([panel runModal] == NSModalResponseOK) {
		return std::string([[panel.URL path] UTF8String]);
	}
	return "";
}

std::string MacOSPlatform::saveFileDialogImpl(const std::string& filter, const std::string& defaultDir) {
	NSSavePanel* panel = [NSSavePanel savePanel];

	if (!filter.empty()) {
		NSString* ext  = [NSString stringWithUTF8String:filter.c_str()];
		UTType*	  type = [UTType typeWithFilenameExtension:ext];
		[panel setAllowedContentTypes:@[ type ]];
	}

	if (!defaultDir.empty()) {
		NSString* dir = [NSString stringWithUTF8String:defaultDir.c_str()];
		[panel setDirectoryURL:[NSURL fileURLWithPath:dir isDirectory:YES]];
	}

	if ([panel runModal] == NSModalResponseOK) {
		return std::string([[panel.URL path] UTF8String]);
	}
	return "";
}

std::vector<std::string> MacOSPlatform::getFileListImpl(const std::string& directory, const std::string& filter) {
	std::vector<std::string> files;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
		if (entry.path().extension() == filter) {
			files.push_back(entry.path().string());
		}
	}
	return files;
}

int MacOSPlatform::showConfirmDialogImpl(const std::string& message, const std::string& confirm, const std::string& deny, const std::string& cancel) {
	NSAlert* alert = [[NSAlert alloc] init];
	[alert setMessageText:[NSString stringWithUTF8String:message.c_str()]];
	[alert setAlertStyle:NSAlertStyleWarning];

	[alert addButtonWithTitle:[NSString stringWithUTF8String:confirm.c_str()]];
	[alert addButtonWithTitle:[NSString stringWithUTF8String:cancel.c_str()]];
	[alert addButtonWithTitle:[NSString stringWithUTF8String:deny.c_str()]];

	NSModalResponse response = [alert runModal];
	return (int)response;
}

void MacOSPlatform::buildScriptLibraryImpl(const std::string& buildDir, const std::string& target, Platform::BuildOutputCallback onOutput, Platform::BuildCompleteCallback onComplete) {
	NSTask* task = [[NSTask alloc] init];
	[task setLaunchPath:[NSString stringWithUTF8String:ATLAS_CMAKE_PATH]];
	[task setArguments:@[
		@"--build", [NSString stringWithUTF8String:buildDir.c_str()],
		@"--target", [NSString stringWithUTF8String:target.c_str()]
	]];

	NSPipe* outputPipe = [NSPipe pipe];
	NSPipe* errorPipe  = [NSPipe pipe];
	[task setStandardOutput:outputPipe];
	[task setStandardError:errorPipe];

	NSFileHandle* outHandle = [outputPipe fileHandleForReading];
	NSFileHandle* errHandle = [errorPipe fileHandleForReading];

	[[NSNotificationCenter defaultCenter]
		addObserverForName:NSFileHandleDataAvailableNotification
					object:outHandle
					 queue:[NSOperationQueue mainQueue]
				usingBlock:^(NSNotification* note) {
				  NSData* data = [outHandle availableData];
				  if (data.length > 0) {
					  std::string line(
						  (const char*)data.bytes, data.length);
					  onOutput(line);
					  [outHandle waitForDataInBackgroundAndNotify];
				  }
				}];

	[[NSNotificationCenter defaultCenter]
		addObserverForName:NSFileHandleDataAvailableNotification
					object:errHandle
					 queue:[NSOperationQueue mainQueue]
				usingBlock:^(NSNotification* note) {
				  NSData* data = [errHandle availableData];
				  if (data.length > 0) {
					  std::string line(
						  (const char*)data.bytes, data.length);
					  onOutput(line);
					  [errHandle waitForDataInBackgroundAndNotify];
				  }
				}];

	[outHandle waitForDataInBackgroundAndNotify];
	[errHandle waitForDataInBackgroundAndNotify];

	[task setTerminationHandler:^(NSTask* t) {
	  bool success = (t.terminationStatus == 0);
	  dispatch_async(dispatch_get_main_queue(), ^{
		onComplete(success);
	  });
	}];

	NSError* error = nil;
	[task launchAndReturnError:&error];
	if (error) {
		onOutput("Failed to launch cmake: " +
				 std::string([[error localizedDescription] UTF8String]));
		onComplete(false);
	}
}

void MacOSPlatform::openFileImpl(const std::string& filepath) {
	// Get the shared NSWorkspace instance
	NSWorkspace* workspace = [NSWorkspace sharedWorkspace];

	// Open the file using the default application
	BOOL success = [workspace openURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:filepath.c_str()]]];

	if (success) {
		NSLog(@"File opened successfully with its default application.");
	} else {
		NSLog(@"Could not open file.");
	}
}

}  // namespace Atlas