#include "atpch.h"
#include "MacOSPlatform.h"

#include "Atlas/Core/Platform.h"

#include <AppKit/AppKit.h>
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CoreFoundation.h>

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

}  // namespace Atlas