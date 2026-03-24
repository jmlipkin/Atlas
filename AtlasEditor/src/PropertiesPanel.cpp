#include "atpch.h"
#include "PropertiesPanel.h"

#include "Atlas/Core/Platform.h"
#include "Atlas/Core/ScriptRegistry.h"
#include "Atlas/Project/Project.h"
#include "Atlas/ImGui/EditorWidgets.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Components/Collision.h"
#include "Atlas/ECS/Systems/Systems.h"

#include <imgui/imgui.h>

namespace Atlas {

void PropertiesPanel::onImGuiRender(Entity& selection) {
	ImGui::Begin("Properties");
	EditorWidgets::DrawPanelAccentBar(EditorWidgets::PanelAccent::Purple);

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

	m_animationEditor.onImGuiRender();
	if (m_selectionType == SelectionType::AnimationClip && m_animationEditor.isClipNameChanged()) {
		m_selectedClip = m_animationEditor.getClipName();
	}

	ImGui::End();
}

void PropertiesPanel::drawComponents(Entity& entity) {
	auto& UUID = entity.getComponent<Component::UUID>().id;
	ImGui::Text("UUID: 0x%016llX", (uint64_t)UUID);

	ImVec2 componentSpacer = {0, 16.0f * EditorWidgets::displayScale};

	drawComponent<Component::Transform>("Transform", entity, [this, &entity](auto& component) {
		float columnWidth = 85.0f;
		float valueWidth  = 50.0f;

		glm::vec2 position = {component.position.x, component.position.y};
		if (EditorWidgets::drawVec2Control<glm::vec2>("Position", position, 0.0f, 0.0f, columnWidth, valueWidth)) {
			component.position = {position, component.position.z};
		}

		glm::vec2 origin = System::Transformation::getCenter2D(entity);
		if (EditorWidgets::drawVec2Control<glm::vec2>("Origin", origin, 0.0f, 0.0f, columnWidth, valueWidth)) {
			System::Transformation::setCenter(entity, {origin.x, origin.y});
		}

		ImGui::Dummy(ImVec2(0, 4.0f * EditorWidgets::displayScale));

		float lineHeight  = ImGui::GetFrameHeight();
		float totalHeight = lineHeight + 2.0f * ImGui::GetStyle().ItemSpacing.y;

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (totalHeight - lineHeight) * 0.5f);
		ImGui::Text("Depth");
		ImGui::NextColumn();

		ImGuiIO io		   = ImGui::GetIO();
		ImFont* boldFont   = io.Fonts->Fonts[0];
		ImVec2	buttonSize = {lineHeight, lineHeight};

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
		ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::purple);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::purpleLight);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorWidgets::purpleActive);
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) component.position.z = 1.0f;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::PushItemWidth(valueWidth);
		ImGui::DragFloat("##Z", &component.position.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);
	});

	ImGui::Dummy(componentSpacer);

	drawComponent<Component::Sprite>("Sprite", entity, [this](auto& component) {
		SubTextureSpecification& specs	  = component.specs;
		const std::string&		 filepath = ProjectManager::toRelativePath(component.texturePath);

		float changeButtonWidth = 62.0f;
		float padding			= 4.0f;
		float maxWidth			= ImGui::GetContentRegionAvail().x - changeButtonWidth - padding * 2.0f;

		std::string displayPath = filepath.empty() ? "No texture" : filepath;
		if (!filepath.empty() && ImGui::CalcTextSize(filepath.c_str()).x > maxWidth) {
			std::string truncated = std::filesystem::path(filepath).filename().string();
			while (!truncated.empty() &&
				   ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth) {
				truncated.pop_back();
			}
			displayPath = truncated + "...";
		}

		ImGui::Text("%s", displayPath.c_str());
		if (ImGui::IsItemHovered() && !filepath.empty()) {
			ImGui::SetTooltip("%s", filepath.c_str());
		}

		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - padding);

		if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
			component.texturePath = Platform::openFileDialog("png");
			ProjectManager::saveScene(m_scene);
		}
		ImGui::Dummy(ImVec2(0, padding));

		bool changed = false;

		float columnWidth = 85.0f;
		float valueWidth  = 50.0f;
		changed |= EditorWidgets::drawVec2Control<glm::vec2>("Size (tiles)", specs.sizeInTiles, 0, 0, columnWidth, valueWidth);
		changed |= EditorWidgets::drawVec2Control<glm::vec2>("Grid Size", specs.tileSize, 0, 0, columnWidth, valueWidth);

		changed |= EditorWidgets::drawVec2Control<glm::ivec2>("Index", specs.index, 0, 0, columnWidth, valueWidth);

		ImGui::Dummy(ImVec2(0, padding));

		if (changed) {
			component.recalculateCoordinates();
			ProjectManager::saveScene(m_scene);
		}
	});

	ImGui::Dummy(componentSpacer);

	drawComponent<Component::Collider>("Collider", entity, [this, &entity](auto& component) {
		ImGui::Checkbox("Show Debug", &m_showCollider);

		bool  changed	  = false;
		float columnWidth = 85.0f;
		float valueWidth  = 50.0f;

		float lineHeight  = ImGui::GetFrameHeight();
		float totalHeight = lineHeight + 2.0f * ImGui::GetStyle().ItemSpacing.y;

		changed |= ImGui::Checkbox("Trigger", &component.isTrigger);

		ImGui::PushID(("##" + entity.getUUID().toString() + "Shape").c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (totalHeight - lineHeight) * 0.5f);
		ImGui::Text("Shape");
		ImGui::NextColumn();

		ImGui::PushItemWidth(columnWidth);
		static const char* items[] = {"AABB", "Circle"};
		int				   value   = (int)component.shape;
		if (ImGui::Combo("##Shape", &value, items, IM_ARRAYSIZE(items))) {
			component.shape = (ColliderShape)value;
			changed			= true;
		}
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();
		ImGui::PushID(("##" + entity.getUUID().toString() + "Size").c_str());
		changed |= EditorWidgets::drawVec2Control<glm::vec2>("Offset", component.offset, 0, 0, columnWidth, valueWidth, false);
		if (component.shape == ColliderShape::AABB) {
			changed |= EditorWidgets::drawVec2Control<glm::vec2>("Size", component.size.AABB, 0, 0, columnWidth, valueWidth);
		} else {
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (totalHeight - lineHeight) * 0.5f);
			ImGui::Text("Radius");
			ImGui::NextColumn();
			ImGuiIO io		   = ImGui::GetIO();
			ImFont* boldFont   = io.Fonts->Fonts[0];
			ImVec2	buttonSize = {lineHeight, lineHeight};

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
			ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::purple);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::purpleLight);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorWidgets::purpleActive);
			ImGui::PushFont(boldFont);
			if (ImGui::Button("R", buttonSize)) component.size.radius = 1.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			ImGui::PushItemWidth(valueWidth);
			ImGui::DragFloat("##Z", &component.size.radius, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::PopStyleVar();
			ImGui::Columns(1);
		}

		ImGui::PopID();

		if (changed) {
			ProjectManager::saveScene(m_scene);
		}
	});

	ImGui::Dummy(componentSpacer);

	drawComponent<Component::RigidBody>("RigidBody", entity, [this, &entity](auto& component) {
		bool  changed	  = false;
		float columnWidth = 85.0f;
		float valueWidth  = 50.0f;

		float lineHeight  = ImGui::GetFrameHeight();
		float totalHeight = lineHeight + 2.0f * ImGui::GetStyle().ItemSpacing.y;

		changed |= ImGui::Checkbox("Static", &component.isStatic);

		ImGui::PushID(("##" + entity.getUUID().toString() + "Shape").c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 120);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (totalHeight - lineHeight) * 0.5f);
		ImGui::Text("Response Type");
		ImGui::NextColumn();

		ImGui::PushItemWidth(columnWidth);
		static const char* items[] = {"None", "Discrete"};
		int				   value   = (int)component.responseType;
		if (ImGui::Combo("##ResponseType", &value, items, IM_ARRAYSIZE(items))) {
			component.responseType = (CollisionResponse)value;
			changed				   = true;
		}
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();

		changed |= EditorWidgets::drawVec2Control<glm::vec2>("Velocity", component.velocity, 0, 0, columnWidth, valueWidth);

		if(changed) {
			ProjectManager::saveScene(m_scene);
		}
	});

	ImGui::Dummy(componentSpacer);

	drawComponent<Component::Animations>("Animations", entity, [this](auto& component) {
		if (ImGui::Button("Add clip")) {
			AnimationClip clip;
			if (!component.clips.contains("Unnamed animation")) {
				component.clips["Unnamed animation"] = clip;
				m_selectedClip						 = "Unnamed animation";
				m_renameTarget						 = "Unnamed animation";
				m_selectionType						 = SelectionType::AnimationClip;
				m_focusRenameCursor					 = 2;
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
			if (!m_renamedClip.empty() && clipName == m_selectedClip && !component.clips.contains(m_renamedClip)) {
				auto node  = component.clips.extract(clipName);
				node.key() = m_renamedClip;
				component.clips.insert(std::move(node));

				if (component.activeClip == clipName) {
					component.activeClip = m_renamedClip;
				}
				m_selectedClip = m_renamedClip;
				m_animationEditor.setClipName(m_renamedClip);
				m_renamedClip = {};
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

			if (m_animationEditorShouldOpen) {
				m_animationEditorShouldOpen = false;
				m_animationEditor.open(&component, m_selectedClip, m_scene);
			}
		}
	});

	ImGui::Dummy(componentSpacer);

	drawComponent<Component::Script>("Script", entity, [this, &entity](auto& component) {
		if (!component.instance) {
			auto names = ScriptRegistry::getRegisteredNames();
			if (ImGui::BeginCombo("##scriptpicker", "Select Script...")) {
				for (auto& name : names) {
					if (ImGui::Selectable(name.c_str())) {
						component.instance = ScriptRegistry::create(name);
						component.priority = ScriptRegistry::getPriority(name);
						component.instance->setEntity(entity);
						component.instance->onCreate();
						component.instance->exposeProperties();
						ProjectManager::saveScene(m_scene);
					}
				}
				if (ImGui::Button("Create New Script")) {
					std::string defaultDir = "";
					if (ProjectManager::getActiveProject()) {
						defaultDir = ProjectManager::getActiveProject()->getData().src_directory;
					}
					std::string path = Platform::saveFileDialog("h", defaultDir);

					if (!path.empty()) {
						std::string className = std::filesystem::path(path).stem().string();
						Behavior::generateNewScript(path);
						Platform::openFile(path);

						if (m_onNewScript) {
							m_onNewScript(entity, className);
						}
					}
				}
				ImGui::EndCombo();
			}
		} else {
			ImGui::Text("Active Script: %s", component.instance->getTypeName().c_str());
			ImGui::SameLine();

			float changeButtonWidth = 62.0f;
			float padding			= 4.0f;
			float maxWidth			= ImGui::GetContentRegionAvail().x - changeButtonWidth - padding * 2.0f;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - padding);

			bool changing = false;
			if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
				component.instance = nullptr;
				changing		   = true;
			}
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 4.0f));

			if (!changing) drawScriptProperties(*component.instance.get());
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
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, EditorWidgets::green);
			ImGui::PushStyleColor(ImGuiCol_Header, EditorWidgets::greenSub);  // greenSub dimmed
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(EditorWidgets::greenSub.x, EditorWidgets::greenSub.y, EditorWidgets::greenSub.z, EditorWidgets::greenSub.w * 0.5f));
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

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
		m_selectedClip	= clip;
		m_selectionType = type;
		if (type == SelectionType::AnimationClip) {
			m_animationEditorShouldOpen = true;
		}
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

void PropertiesPanel::drawScriptProperties(Behavior& behavior) {
	auto& properties = behavior.getProperties();
	for (auto& [name, property] : properties) {
		float maxWidth = 85.0f;

		ImGui::PushID((name + property.name).c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, maxWidth);

		std::string label = name;
		if (ImGui::CalcTextSize(name.c_str()).x > maxWidth) {
			while (!label.empty() &&
				   ImGui::CalcTextSize((label + "...").c_str()).x > maxWidth) {
				label.pop_back();
			}
			label = label + "...";
		}

		ImGui::Text("%s", label.c_str());
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", name.c_str());
		}

		ImGui::NextColumn();

		drawScriptProperty(property);

		ImGui::Columns(1);
		ImGui::PopID();
	}
}

void PropertiesPanel::drawScriptProperty(Behavior::Property& property) {
	bool changed = false;
	switch (property.type) {
		case BehaviorPropertyType::BOOL: {
			changed = ImGui::Checkbox(("##" + property.name).c_str(), (bool*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::CHAR: {
			break;
		}
		case BehaviorPropertyType::STRING: {
			changed = ImGui::InputText(("##" + property.name).c_str(), (std::string*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::INT: {
			changed = ImGui::DragInt(("##" + property.name).c_str(), (int*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::FLOAT: {
			changed = ImGui::DragFloat(("##" + property.name).c_str(), (float*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::VEC2: {
			changed = EditorWidgets::drawVec2Control(("##" + property.name).c_str(), *(glm::vec2*)property.valuePtr, 0, 0, 0);
			break;
		}
		case BehaviorPropertyType::VEC3: {
			changed = EditorWidgets::drawVec3Control(("##" + property.name).c_str(), *(glm::vec3*)property.valuePtr, 0, 0, 0, 0);
			break;
		}
		case BehaviorPropertyType::VEC4: {
			break;
		}
		case BehaviorPropertyType::MAT4: {
			break;
		}
	}
	if (changed) {
		ProjectManager::saveScene(m_scene);
	}
}

}  // namespace Atlas