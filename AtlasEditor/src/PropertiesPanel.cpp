#include "atpch.h"
#include "PropertiesPanel.h"

#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"
#include "Atlas/ImGui/ImGuiSystem.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Animation.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Atlas {

void PropertiesPanel::onImGuiRender(Entity& selection) {
	ImGui::Begin("Properties");
	ImGuiSystem::DrawPanelAccentBar(ImGuiSystem::PanelAccent::Purple);

	if (selection)
		drawComponents(selection);

	if (ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(0) && !ImGui::IsAnyItemHovered()) {
		if (m_renameTarget.empty()) {
			m_selectedClip	= {};
			m_selectionType = SelectionType::None;
		}
	}

	if (m_justFinishedRename) {
		m_justFinishedRename = false;
	} else if (ImGui::IsWindowFocused() && !m_selectedClip.empty() && m_renameTarget.empty() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
		m_renameTarget		= m_selectedClip;
		m_focusRenameCursor = 2;
	}

	ImGui::End();
}

void PropertiesPanel::drawComponents(Entity& entity) {
	auto& UUID = entity.getComponent<Component::UUID>().id;
	ImGui::Text("UUID: 0x%016llX", (uint64_t)UUID);

	drawComponent<Component::Transform>("Transform", entity, [this](auto& component) {
		drawVec3Control("Position", component.position, 0.0f, 0.0f, 1.0f, 120.0f);
	});
	drawComponent<Component::Sprite>("Sprite", entity, [this](auto& component) {
		SubTextureSpecification& specs	  = component.specs;
		const std::string&		 filepath = ProjectManager::toRelativePath(component.texturePath);

		ImGui::Text("%s", filepath.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Change")) {
			component.texturePath = Platform::openFileDialog("png");
			ProjectManager::saveScene(m_scene);
		}

		bool changed = false;
		changed |= drawVec2Control<glm::vec2>("Size (tiles)", specs.sizeInTiles);
		changed |= drawVec2Control<glm::vec2>("Grid Size", specs.tileSize);

		changed |= drawVec2Control<glm::ivec2>("Index", specs.index);

		if (changed) {
			component.recalculateCoordinates();
			ProjectManager::saveScene(m_scene);
		}
	});
	drawComponent<Component::Animations>("Animations", entity, [this](auto& component) {
		if (ImGui::Button("Add clip")) {
			AnimationClip clip;
			if (!component.clips.contains("Unnamed animation")) {
				component.clips["Unnamed animation"] = clip;
				m_selectedClip = "Unnamed animation";
				m_renameTarget = "Unnamed animation";
				m_selectionType = SelectionType::AnimationClip;
				m_focusRenameCursor = 2;
				ProjectManager::saveScene(m_scene);
			} else {
				AT_CORE_WARN("Clips cannot shared the same name!\n\t\tSolution: Provide a name for clip: \"Unnamed animation\"");
			}
		}
		if (component.clips.empty()) {
			ImGui::Text("No clips exist!");
		}

		for (auto& [clipName, clip] : component.clips) {
			std::string editableName = clipName;
			drawClipLabel(editableName, SelectionType::AnimationClip);
			if (!m_renamedClip.empty() && clipName == m_selectedClip) {
				auto node  = component.clips.extract(clipName);
				node.key() = m_renamedClip;
				component.clips.insert(std::move(node));

				if (component.activeClip == clipName) {
					component.activeClip = m_renamedClip;
				}
				m_selectedClip = m_renamedClip;
				m_renamedClip  = {};
				ProjectManager::saveScene(m_scene);
				break;
			}

			if (!m_clipToDelete.empty()) {
				if (component.clips.contains(m_clipToDelete)) {
					component.clips.erase(m_clipToDelete);
				}
				if (m_selectedClip == m_clipToDelete) {
					m_selectedClip = {};
				}
				m_clipToDelete = {};
				ProjectManager::saveScene(m_scene);
				break;
			}
		}
	});
}

void PropertiesPanel::drawClipLabel(std::string& clip, SelectionType type) {
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

	bool wasDimmed = false;
	if (clip == m_selectedClip) {
		tree_flags |= ImGuiTreeNodeFlags_Selected;
		m_selectionType = type;

		bool panelFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		if (!panelFocused) {
			wasDimmed = true;
			// Dimmed selection color when panel is inactive
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImGuiSystem::green);
			ImGui::PushStyleColor(ImGuiCol_Header, ImGuiSystem::greenSub);	// greenSub dimmed
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(ImGuiSystem::greenSub.x, ImGuiSystem::greenSub.y, ImGuiSystem::greenSub.z, ImGuiSystem::greenSub.w * 0.5f));
		}
	}

	bool opened = false;
	if (clip == m_renameTarget) {
		if (m_focusRenameCursor == 2) {
			m_renameBuffer = clip;
		}
		if (m_focusRenameCursor > 0) {
			ImGui::SetKeyboardFocusHere();
			m_focusRenameCursor--;
		}
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputText("##rename", &m_renameBuffer, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
			m_renamedClip  = m_renameBuffer;
			m_renameTarget = {};
			ImGui::SetKeyboardFocusHere(-1);
			m_justFinishedRename = true;
			ProjectManager::saveScene(m_scene);
		}
	} else {
		ImGui::PushID(clip.c_str());
		opened = ImGui::TreeNodeEx("##clip", tree_flags, "%s", clip.c_str());
		ImGui::PopID();
	}

	if (wasDimmed) ImGui::PopStyleColor(3);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
		m_renameTarget		= clip;
		m_focusRenameCursor = 2;
	}

	if (ImGui::IsItemClicked()) {
		m_selectedClip = clip;
	}

	if (clip == m_renameTarget && ImGui::IsItemDeactivated()) {
		m_renameTarget = {};
		ProjectManager::saveScene(m_scene);
	}

	bool clipDeleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Rename")) {
			m_renameTarget		= clip;
			m_focusRenameCursor = 2;
		}
		if (ImGui::MenuItem("Delete"))
			clipDeleted = true;
		ImGui::EndPopup();
	}

	if (opened) {
		ImGui::TreePop();
	}

	if (clipDeleted) {
		m_clipToDelete = clip;
	}
}

bool PropertiesPanel::drawVec3Control(const char* label, glm::vec3& values, float resetX, float resetY, float resetZ, float columnWidth) {
	ImGuiIO io		 = ImGui::GetIO();
	ImFont* boldFont = io.Fonts->Fonts[0];

	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

	float  lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

	// X
	ImGui::PushStyleColor(ImGuiCol_Button, ImGuiSystem::steelBlue);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiSystem::steelBlueLight);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiSystem::steelBlueActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize)) values.x = resetX;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// Y
	ImGui::PushStyleColor(ImGuiCol_Button, ImGuiSystem::green);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiSystem::greenLight);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiSystem::greenActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize)) values.y = resetY;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// Z
	ImGui::PushStyleColor(ImGuiCol_Button, ImGuiSystem::purple);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiSystem::purpleLight);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiSystem::purpleActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize)) values.z = resetZ;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();

	return changed;
}

}  // namespace Atlas