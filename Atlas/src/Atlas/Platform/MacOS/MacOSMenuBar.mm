#include "MacOSMenuBar.h"
#include "atpch.h"

#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"

#include <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <filesystem>
#include <cstddef>
#include <Foundation/Foundation.h>

@interface AtlasMenuBarDelegate : NSObject
- (void)newProject:(id)sender;
- (void)openProject:(id)sender;
- (void)saveProject:(id)sender;
- (void)saveProjectAs:(id)sender;
- (BOOL)validateMenuItem:(NSMenuItem *)menuItem;

@end

@implementation AtlasMenuBarDelegate

- (void)newProject:(id)sender {
	std::string path = Atlas::Platform::saveFileDialog("atproj");
	if (path.empty()) return;

	std::string name = std::filesystem::path(path).stem().string();
	Atlas::ProjectManager::createNewProject(path, name);
}

- (void)openProject:(id)sender {
	std::string path = Atlas::Platform::openFileDialog("atproj");
	if (path.empty()) return;
	Atlas::ProjectManager::loadProject(path);
}

- (void)saveProject:(id)sender {
	Atlas::ProjectManager::saveProject();
}

- (void)saveProjectAs:(id)sender {
	std::string path = Atlas::Platform::saveFileDialog("atproj");
	if (path.empty()) return;
	// TODO: prompt for name, hardcode for now
	Atlas::ProjectManager::saveProjectAs(path, Atlas::ProjectManager::getActiveProject()->getName());
}

- (BOOL)validateMenuItem:(NSMenuItem*)menuItem {
	return YES;
}

@end

namespace Atlas {

void MacOSMenuBar::generateMenuBar(const std::string& title) {
	NSMenu* menuBar = [[NSMenu alloc] init];

	/////////////////////////////////////////////

	NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
	[menuBar addItem:appMenuItem];
	[NSApp setMainMenu:menuBar];

	NSString* appName = [NSString stringWithCString:title.c_str() encoding:[NSString defaultCStringEncoding]];
	NSMenu*	  appMenu = [[NSMenu alloc] initWithTitle:appName];

	NSString*	quitTitle = @"Quit Atlas Editor";
	NSMenuItem* quitItem =
		[[NSMenuItem alloc] initWithTitle:quitTitle
								   action:@selector(terminate:)
							keyEquivalent:@"q"];
	[appMenu addItem:[NSMenuItem separatorItem]];
	[appMenu addItem:quitItem];
	[appMenuItem setSubmenu:appMenu];

	/////////////////////////////////////////////

	NSMenuItem* fileMenuItem = [[NSMenuItem alloc] init];
	[menuBar addItem:fileMenuItem];
	NSMenu* fileMenu = [[NSMenu alloc] initWithTitle:@"File"];

	AtlasMenuBarDelegate* delegate = [[AtlasMenuBarDelegate alloc] init];

	NSMenuItem* newProject = [[NSMenuItem alloc] initWithTitle:@"Create Project" action:@selector(newProject:) keyEquivalent:@"N"];
	[newProject setTarget:delegate];
	[fileMenu addItem:newProject];

	NSMenuItem* openProject = [[NSMenuItem alloc] initWithTitle:@"Open Project" action:@selector(openProject:) keyEquivalent:@"O"];
	[openProject setTarget:delegate];
	[fileMenu addItem:openProject];

	[fileMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem* saveProject = [[NSMenuItem alloc] initWithTitle:@"Save Project" action:@selector(saveProject:) keyEquivalent:@"s"];
	[saveProject setTarget:delegate];
	[fileMenu addItem:saveProject];

	NSMenuItem* saveProjectAs = [[NSMenuItem alloc] initWithTitle:@"Save Project As" action:@selector(saveProjectAs:) keyEquivalent:@"S"];
	[saveProjectAs setTarget:delegate];
	[fileMenu addItem:saveProjectAs];

	[fileMenuItem setSubmenu:fileMenu];
}

}  // namespace Atlas