#include "MacOSMenuBar.h"
#include "atpch.h"

#include <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <cstddef>
#include <Foundation/Foundation.h>

namespace Atlas {

void MacOSMenuBar::generateMenuBar(const std::string& title) {
	NSMenu* menuBar = [[NSMenu alloc] init];

	/////////////////////////////////////////////

	NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
	[menuBar addItem:appMenuItem];
	[NSApp setMainMenu:menuBar];

	NSString* appName = [NSString stringWithCString:title.c_str() encoding:[NSString defaultCStringEncoding]];
	NSMenu* appMenu = [[NSMenu alloc] initWithTitle:appName];

	NSString* quitTitle = @"Quit Atlas Editor";
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

	NSString* newSceneTitle = @"New Scene";
	NSMenuItem* newItem = [[NSMenuItem alloc] initWithTitle:newSceneTitle action:NULL keyEquivalent:@"n"];
	[fileMenu addItem:newItem];
	NSString* openTitle = @"Open";
	NSMenuItem* openItem = [[NSMenuItem alloc] initWithTitle:openTitle action:NULL keyEquivalent:@"o"];
	[fileMenu addItem:openItem];

	[fileMenu addItem:[NSMenuItem separatorItem]];

	NSString* saveSceneTitle = @"Save Scene";
	NSString* saveSceneAsTitle = @"Save Scene As";
	NSMenuItem* saveItem =
		[[NSMenuItem alloc] initWithTitle:saveSceneTitle
								   action:NULL
							keyEquivalent:@"s"];
	NSMenuItem* saveAsItem =
		[[NSMenuItem alloc] initWithTitle:saveSceneAsTitle
								   action:NULL
							keyEquivalent:@"S"];


	[fileMenu addItem:saveItem];
	[fileMenu addItem:saveAsItem];
	[fileMenuItem setSubmenu:fileMenu];
}

}  // namespace Atlas