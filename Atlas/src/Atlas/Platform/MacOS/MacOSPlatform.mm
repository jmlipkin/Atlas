#include "atpch.h"
#include "MacOSPlatform.h"

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

std::string MacOSPlatform::saveFileDialogImpl(const std::string& filter) {
	NSSavePanel* panel = [NSSavePanel savePanel];

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

}  // namespace Atlas