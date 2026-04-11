#pragma once

#include "Commands/CommandHistory.h"
#include "PropertiesPanel.h"

#include "Atlas/Scene/Scene.h"

namespace Atlas {

class SceneHierarchyPanel {
  public:
	SceneHierarchyPanel(std::shared_ptr<Scene> scene, CommandHistory& commandHistory) : m_scene(scene), m_commandHistory(commandHistory), m_propertiesPanel(scene) {}
	~SceneHierarchyPanel() = default;

	void setScene(std::shared_ptr<Scene> scene);
	void onImGuiRender();

	void addEmptyEntity();
	void addComponent() { m_openComponentPicker = true; }

	Entity&		  getSelectionContext() { return m_selectionContext; }
	const Entity& getSelectionContext() const { return m_selectionContext; }
	void		  clearSelection() {
		 m_selectionContext = {};
		 m_renameTarget		= {};
		 m_renameBuffer		= {};
		 m_propertiesPanel.clearSelection();
	}

	bool showSelectedCollider() const { return m_propertiesPanel.showSelectedCollider(); }

	void setOnNewScript(PropertiesPanel::NewScriptCallback callback) { m_propertiesPanel.setOnNewScriptCallback(callback); }

  private:
	void drawEntityNode(Entity& entity);
	void drawComponentPicker(Entity& entity);

	void autoSave();

  private:
	std::shared_ptr<Scene> m_scene;
	CommandHistory&		   m_commandHistory;

	PropertiesPanel m_propertiesPanel;

	Entity m_selectionContext;
	Entity m_renameTarget;

	std::string m_renameBuffer = {};

	bool m_openComponentPicker = false;
	int	 m_focusRenameCursor   = 0;
	bool m_justFinishedRename  = false;
};

}  // namespace Atlas