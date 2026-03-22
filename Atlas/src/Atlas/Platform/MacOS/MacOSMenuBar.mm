#include "atpch.h"
#include "MacOSMenuBar.h"

#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"

#include <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#include <filesystem>
#include <cstddef>
#include <Foundation/Foundation.h>

@interface AtlasMenuBarDelegate : NSObject

@property(nonatomic, assign) Atlas::MenuBar::ProjectChangedCallback onProjectChanged;
@property(nonatomic, assign) Atlas::MenuBar::SceneLoadedCallback	onSceneLoaded;
@property(nonatomic, assign) Atlas::MenuBar::SceneSavedCallback		onSceneSaved;
@property(nonatomic, assign) Atlas::MenuBar::SceneCreatedCallback	onNewScene;
@property(nonatomic, assign) Atlas::MenuBar::ProjectClosedCallback	onProjectClosed;
@property(nonatomic, assign) Atlas::MenuBar::SceneClosedCallback	onSceneClosed;

@property(nonatomic, assign) Atlas::MenuBar::NewEntityCallback	  onNewEntity;
@property(nonatomic, assign) Atlas::MenuBar::AddComponentCallback onAddComponent;

@property(nonatomic, assign) Atlas::MenuBar::PreviewCallback onPreview;

@property(nonatomic, assign) Atlas::MenuBar::ValidationCallback onValidateProjectRequired;
@property(nonatomic, assign) Atlas::MenuBar::ValidationCallback onValidateSceneRequired;

- (void)newProject:(id)sender;
- (void)newScene:(id)sender;
- (void)openProject:(id)sender;
- (void)closeProject:(id)sender;
- (void)saveProject:(id)sender;
- (void)saveProjectAs:(id)sender;
- (void)saveScene:(id)sender;
- (void)openScene:(id)sender;

- (void)addEntity:(id)sender;
- (void)addComponent:(id)sender;
- (void)onPreview:(id)sender;

- (BOOL)validateMenuItem:(NSMenuItem*)menuItem;

@end

@implementation AtlasMenuBarDelegate

- (void)newProject:(id)sender {
	std::string path = Atlas::Platform::saveFileDialog("atproj");
	if (path.empty()) return;

	std::string name	  = std::filesystem::path(path).stem().string();
	std::string directory = std::filesystem::path(path).parent_path().string();
	Atlas::ProjectManager::createNewProject(directory, name);
	if (self.onProjectChanged) self.onProjectChanged(directory + "/" + name);
}

- (void)newScene:(id)sender {
	std::string path = Atlas::Platform::saveFileDialog("atscene");
	if (path.empty()) return;

	std::string					  name	= std::filesystem::path(path).stem().string();
	std::shared_ptr<Atlas::Scene> scene = Atlas::ProjectManager::createNewScene(path, name);
	if (self.onNewScene) self.onNewScene(scene);
}

- (void)openProject:(id)sender {
	std::string path = Atlas::Platform::openFileDialog("atproj");
	if (path.empty()) return;

	std::string name	  = std::filesystem::path(path).stem().string();
	std::string directory = std::filesystem::path(path).parent_path().string();

	std::shared_ptr<Atlas::Scene> scene = Atlas::ProjectManager::loadProject(path);
	if (self.onSceneLoaded && scene) self.onSceneLoaded(scene);
	if (self.onProjectChanged) self.onProjectChanged(directory + "/" + name);
}

- (void)closeProject:(id)sender {
	if (Atlas::ProjectManager::isDirty()) {
		int result = Atlas::Platform::showConfirmDialog("Do you want to save changes to your project?", "Save", "Don't Save", "Cancel");
		if (result == NSAlertFirstButtonReturn) {
			Atlas::ProjectManager::closeProject(true);
		} else if (result == NSAlertThirdButtonReturn) {
			Atlas::ProjectManager::closeProject(false);
		}
	} else {
		// Project has not changed, so no need to save
		Atlas::ProjectManager::closeProject(false);
	}
	if (self.onProjectClosed) self.onProjectClosed();
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

- (void)saveScene:(id)sender {
	if (self.onSceneSaved) self.onSceneSaved();
}

- (void)openScene:(id)sender {
	std::string path = Atlas::Platform::openFileDialog("atscene");
	if (path.empty()) return;
	std::shared_ptr<Atlas::Scene> scene = Atlas::ProjectManager::loadScene(path);
	if (self.onSceneLoaded) self.onSceneLoaded(scene);
}

- (void)addEntity:(id)sender {
	if (self.onNewEntity) self.onNewEntity();
}

- (void)addComponent:(id)sender {
	if (self.onAddComponent) self.onAddComponent();
}

- (void)onPreview:(id)sender {
	if (self.onPreview) self.onPreview();
}

- (BOOL)validateMenuItem:(NSMenuItem*)menuItem {
	SEL action = menuItem.action;

	if (action == @selector(saveScene:) ||
		action == @selector(saveProject:) ||
		action == @selector(saveProjectAs:) ||
		action == @selector(newScene:) ||
		action == @selector(preview:)) {
		return self.onValidateSceneRequired ? self.onValidateSceneRequired() : NO;
	}

	if (action == @selector(closeProject:) ||
		action == @selector(saveProject:)) {
		return self.onValidateProjectRequired ? self.onValidateProjectRequired() : NO;
	}

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
	delegate.onProjectChanged	   = m_onProjectChanged;
	delegate.onSceneLoaded		   = m_onSceneLoaded;
	delegate.onSceneSaved		   = m_onSceneSaved;
	delegate.onNewScene			   = m_onNewScene;
	delegate.onProjectClosed	   = m_onProjectClosed;
	delegate.onSceneClosed		   = m_onSceneClosed;

	delegate.onNewEntity	= m_onNewEntity;
	delegate.onAddComponent = m_onAddComponent;

	delegate.onPreview = m_onPreview;
	
	delegate.onValidateSceneRequired = m_onSceneValidation;
	delegate.onValidateProjectRequired = m_onProjectValidation;

	NSMenuItem* newProject = [[NSMenuItem alloc] initWithTitle:@"New Project" action:@selector(newProject:) keyEquivalent:@"N"];
	[newProject setTarget:delegate];
	[fileMenu addItem:newProject];

	NSMenuItem* newScene = [[NSMenuItem alloc] initWithTitle:@"New Scene" action:@selector(newScene:) keyEquivalent:@"n"];
	[newScene setTarget:delegate];
	[fileMenu addItem:newScene];

	[fileMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem* openProject = [[NSMenuItem alloc] initWithTitle:@"Open Project" action:@selector(openProject:) keyEquivalent:@"O"];
	[openProject setTarget:delegate];
	[fileMenu addItem:openProject];

	NSMenuItem* openScene = [[NSMenuItem alloc] initWithTitle:@"Open Scene" action:@selector(openScene:) keyEquivalent:@""];
	[openScene setTarget:delegate];
	[fileMenu addItem:openScene];

	[fileMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem* saveProject = [[NSMenuItem alloc] initWithTitle:@"Save Project" action:@selector(saveProject:) keyEquivalent:@"S"];
	[saveProject setTarget:delegate];
	[fileMenu addItem:saveProject];

	NSMenuItem* saveProjectAs = [[NSMenuItem alloc] initWithTitle:@"Save Project As" action:@selector(saveProjectAs:) keyEquivalent:@"S"];
	[saveProjectAs setTarget:delegate];
	[saveProjectAs setKeyEquivalentModifierMask:NSEventModifierFlagOption | NSEventModifierFlagCommand];
	[fileMenu addItem:saveProjectAs];

	NSMenuItem* saveScene = [[NSMenuItem alloc] initWithTitle:@"Save Scene" action:@selector(saveScene:) keyEquivalent:@"s"];
	[saveScene setTarget:delegate];
	[fileMenu addItem:saveScene];

	[fileMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem* closeProject = [[NSMenuItem alloc] initWithTitle:@"Close Project" action:@selector(closeProject:) keyEquivalent:@"W"];
	[closeProject setTarget:delegate];
	[fileMenu addItem:closeProject];

	[fileMenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem* preview = [[NSMenuItem alloc] initWithTitle:@"Preview" action:@selector(onPreview:) keyEquivalent:@"p"];
	[preview setTarget:delegate];
	[fileMenu addItem:preview];

	[fileMenuItem setSubmenu:fileMenu];

	/////////////////////////////////////////////

	NSMenuItem* editMenuItem = [[NSMenuItem alloc] init];
	[menuBar addItem:editMenuItem];
	NSMenu* editMenu = [[NSMenu alloc] initWithTitle:@"Edit"];

	NSMenuItem* addEntity = [[NSMenuItem alloc] initWithTitle:@"Add Entity" action:@selector(addEntity:) keyEquivalent:@"e"];
	[addEntity setTarget:delegate];
	[editMenu addItem:addEntity];

	NSMenuItem* addComponent = [[NSMenuItem alloc] initWithTitle:@"Add Component" action:@selector(addComponent:) keyEquivalent:@"a"];
	[addComponent setTarget:delegate];
	[editMenu addItem:addComponent];

	[editMenuItem setSubmenu:editMenu];
}

}  // namespace Atlas