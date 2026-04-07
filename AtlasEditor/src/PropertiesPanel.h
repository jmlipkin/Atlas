#pragma once

#include "AnimationClipPanel.h"
#include "TilemapEditorPanel.h"

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ImGui/EditorWidgets.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Atlas {

class PropertiesPanel {
  public:
	using NewScriptCallback = std::function<void(Entity, const std::string& scriptName)>;

	PropertiesPanel(std::shared_ptr<Scene> scene) : m_scene(scene) {}
	~PropertiesPanel() = default;

	void onImGuiRender(Entity& selection);
	void setScene(std::shared_ptr<Scene> scene) {
		m_scene = scene;
		m_animationEditor.close();
	}

	bool showSelectedCollider() const { return m_showCollider; }
	void setOnNewScriptCallback(NewScriptCallback callback) { m_onNewScript = callback; }

	void setTilemapOpenState(bool state) { m_firstOpenedTilemap = state; }

  private:
	enum class SelectionType { None,
							   AnimationClip,
							   AudioClip };

	template <typename T, typename UIFunction>
	void drawComponent(const char* label, Entity& entity, UIFunction uiFunction) {
		if (!entity.hasComponent<T>())
			return;

		auto&					 component	   = entity.getComponent<T>();
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;

		// TODO: Update formatting
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float  padding				  = 2.0 * EditorWidgets::displayScale;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{padding, padding});
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", label);
		ImGui::PopStyleVar();

		float lineHeight = ImGui::GetFrameHeight();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

		std::string popupID = std::string("ComponentSettings##") + typeid(T).name();
		if (ImGui::Button("\u2013", ImVec2{lineHeight, lineHeight})) {
			ImGui::OpenPopup(popupID.c_str());
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup(popupID.c_str())) {
			if (ImGui::MenuItem("Delete component"))
				removeComponent = true;
			ImGui::EndPopup();
		}

		if (open) {
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent) {
			entity.removeComponent<T>();
		}
	}

	void drawComponents(Entity& entity);

	void drawClipLabel(std::string& clip, SelectionType type);

	void drawScriptProperties(Behavior& behavior);
	void drawScriptProperty(const char* label, Behavior::Property& property);

  private:
	std::shared_ptr<Scene> m_scene;

	AnimationClipPanel m_animationEditor;
	bool			   m_animationEditorShouldOpen = false;

	SelectionType m_selectionType = PropertiesPanel::SelectionType::None;
	std::string	  m_selectedClip;

	std::string m_clipToDelete;

	std::string m_renameTarget;
	std::string m_renamedClip;
	std::string m_renameBuffer;
	int			m_focusRenameCursor	 = 0;
	bool		m_justFinishedRename = false;

	bool m_showCollider = false;

	NewScriptCallback m_onNewScript;

	std::vector<std::string> m_availableTilesets;
	TilemapEditorPanel		 m_tilemapEditorPanel;

	bool m_firstOpenedTilemap = false;
};

}  // namespace Atlas