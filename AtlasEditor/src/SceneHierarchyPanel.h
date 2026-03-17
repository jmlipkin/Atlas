#pragma once

#include "PropertiesPanel.h"

#include "Atlas/Scene/Scene.h"

namespace Atlas {

class SceneHierarchyPanel {
  public:
	SceneHierarchyPanel(std::shared_ptr<Scene> scene) : m_scene(scene), m_propertiesPanel(scene) {}
	~SceneHierarchyPanel() = default;

	void setScene(std::shared_ptr<Scene> scene);
	void onImGuiRender();

	void addEmptyEntity();
	void addComponent() { m_openComponentPicker = true; }

	const Entity& getSelectionContext() const { return m_selectionContext; }

  private:
	void drawEntityNode(Entity& entity);
	void drawComponentPicker(Entity& entity);

	void autoSave();

  private:
	PropertiesPanel		   m_propertiesPanel;
	std::shared_ptr<Scene> m_scene;

	Entity m_selectionContext;
	Entity m_renameTarget;
	bool   m_openComponentPicker = false;
	int	   m_focusRenameCursor	 = 0;
	bool   m_justFinishedRename	 = false;
};

}  // namespace Atlas