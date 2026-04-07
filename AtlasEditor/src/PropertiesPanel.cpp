#include "atpch.h"
#include "PropertiesPanel.h"

#include "Atlas/Core/Platform.h"
#include "Atlas/Core/ScriptRegistry.h"
#include "Atlas/Project/Project.h"
#include "Atlas/ImGui/EditorWidgets.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Components/Collider.h"
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

	m_tilemapEditorPanel.onImGuiRender();

	m_animationEditor.onImGuiRender();
	if (m_selectionType == SelectionType::AnimationClip && m_animationEditor.isClipNameChanged())
		m_selectedClip = m_animationEditor.getClipName();

	ImGui::End();
}

void PropertiesPanel::drawComponents(Entity& entity) {
	auto& UUID = entity.getComponent<Component::UUID>().id;
	ImGui::Text("UUID: 0x%016llX", (uint64_t)UUID);

	const float	 columnWidth	 = 100.0f;
	const float	 valueWidth		 = 85.0f;
	const ImVec2 componentSpacer = {0, 16.0f};
	const ImVec2 padding4		 = {0, 4.0f};

	// -------------------------------------------------------------------------
	drawComponent<Component::Transform>("Transform", entity, [this, &entity, componentSpacer, padding4, columnWidth, valueWidth](auto& component) {
		glm::vec2 position = {component.position.x, component.position.y};
		if (EditorWidgets::drawVec2Control<glm::vec2>("Position", position, columnWidth, valueWidth, 0, 0, true))
			component.position = {position, component.position.z};

		if (entity.hasComponent<Component::Sprite>()) {
			glm::vec2 origin = System::Transformation::getCenter2D(entity);
			if (EditorWidgets::drawVec2Control<glm::vec2>("Origin", origin, columnWidth, valueWidth))
				System::Transformation::setCenter(entity, {origin.x, origin.y});
		}

		EditorWidgets::drawFloatControl("Depth", component.position.z, "D", columnWidth, valueWidth, 1.0f);
		ImGui::Dummy(componentSpacer);
	});

	// -------------------------------------------------------------------------
	drawComponent<Component::Sprite>("Sprite", entity, [this, componentSpacer, padding4, columnWidth, valueWidth](auto& component) {
		SubTextureSpecification& specs	  = component.specs;
		const std::string&		 filepath = ProjectManager::toRelativePath(component.texturePath);

		float changeButtonWidth = 62.0f;
		float maxWidth			= ImGui::GetContentRegionAvail().x - changeButtonWidth - 8.0f;

		std::string displayPath = filepath.empty() ? "No texture" : filepath;
		if (!filepath.empty() && ImGui::CalcTextSize(filepath.c_str()).x > maxWidth) {
			std::string truncated = std::filesystem::path(filepath).filename().string();
			while (!truncated.empty() && ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth)
				truncated.pop_back();
			displayPath = truncated + "...";
		}

		ImGui::Text("%s", displayPath.c_str());
		if (ImGui::IsItemHovered() && !filepath.empty())
			ImGui::SetTooltip("%s", filepath.c_str());

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f);
		if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
			component.texturePath = Platform::openFileDialog("png");
			ProjectManager::saveScene(m_scene);
		}

		ImGui::Dummy(padding4);

		bool changed = false;
		changed |= EditorWidgets::drawVec2Control<glm::vec2>("Size", specs.sizeInTiles, columnWidth, valueWidth, 1, 1);
		changed |= EditorWidgets::drawVec2Control<glm::ivec2>("Index", specs.index, columnWidth, valueWidth);

		if (changed) {
			component.recalculateCoordinates();
			ProjectManager::saveScene(m_scene);
		}

		ImGui::Dummy(componentSpacer);
	});

	// -------------------------------------------------------------------------
	drawComponent<Component::Tilemap>("Tile Map", entity, [this, &entity, componentSpacer, padding4, columnWidth, valueWidth](auto& component) {
		bool changed = false;

		if (component.tileset.empty()) {
			if (m_firstOpenedTilemap) {
				auto names = Platform::getFileList(ProjectManager::getActiveProject()->getDirectory(), ".attileset");
				for (auto& name : names)
					name = std::filesystem::path(ProjectManager::toRelativePath(name)).stem().string();
				m_availableTilesets	 = names;
				m_firstOpenedTilemap = false;
			}

			if (ImGui::BeginCombo("##tilesetpicker", "Select Tileset...")) {
				for (auto& tileset : m_availableTilesets) {
					if (ImGui::Selectable(tileset.c_str())) {
						component.tileset = tileset;
						changed			  = true;
					}
				}
				if (ImGui::Button("Create New Tileset")) {
					std::string defaultDir = ProjectManager::getActiveProject() ? ProjectManager::getActiveProject()->getData().src_directory : "";
					std::string path	   = Platform::saveFileDialog("attileset", defaultDir);
					if (!path.empty()) {
						ProjectManager::createTileset(path, std::filesystem::path(path).stem().string());
						component.tileset = std::filesystem::path(ProjectManager::toRelativePath(path)).stem().string();
						changed			  = true;
					}
				}
				ImGui::EndCombo();
			}
		} else {
			float changeButtonWidth = 62.0f * EditorWidgets::displayScale;
			ImGui::Text("Tileset: %s", component.tileset.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f * EditorWidgets::displayScale);
			if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
				component.tileset	 = {};
				m_firstOpenedTilemap = true;
			}

			ImGui::Separator();
			ImGui::Dummy(padding4);

			glm::ivec2 size = component.size;
			if (EditorWidgets::drawVec2Control<glm::ivec2>("Size", size, columnWidth, valueWidth)) {
				if (size.x >= 1 && size.y >= 1) {
					component.resize(size);
					changed = true;
				}
			}

			// TODO: Editor ImGui system
			// changed |= EditorWidgets::drawIntControl("Layer Mask", component.layerMask, "L", columnWidth, valueWidth);
			changed |= EditorWidgets::drawCheckbox("Debug Overlay", component.showOverlay, columnWidth);

			ImGui::Dummy(padding4);
			if (ImGui::Button("Open Tilemap Editor"))
				m_tilemapEditorPanel.open(&component, m_scene, &entity);
		}

		if (changed)
			ProjectManager::saveScene(m_scene);

		ImGui::Dummy(componentSpacer);
	});

	// -------------------------------------------------------------------------
	drawComponent<Component::Collider>("Collider", entity, [this, &entity, componentSpacer, columnWidth, valueWidth](auto& component) {
		bool changed = false;

		changed |= EditorWidgets::drawCheckbox("Show Debug", m_showCollider, columnWidth);
		changed |= EditorWidgets::drawCheckbox("Trigger", component.isTrigger, columnWidth);

		static const char* shapeItems[] = {"AABB", "Circle"};
		int				   shapeValue	= (int)component.shape;
		if (EditorWidgets::drawCombo("Shape", shapeItems, 2, shapeValue, columnWidth, columnWidth)) {
			component.shape = (ColliderShape)shapeValue;
			changed			= true;
		}

		EditorWidgets::drawVec2Control<glm::vec2>("Offset", component.offset, columnWidth, valueWidth);

		if (component.shape == ColliderShape::AABB) {
			changed |= EditorWidgets::drawVec2Control<glm::vec2>("Size", component.size.AABB, columnWidth, valueWidth);
		} else {
			changed |= EditorWidgets::drawFloatControl("Radius", component.size.radius, "R", 1.0f, 0.1f, columnWidth);
		}

		if (changed)
			ProjectManager::saveScene(m_scene);

		ImGui::Dummy(componentSpacer);
	});

	// -------------------------------------------------------------------------
	drawComponent<Component::RigidBody>("RigidBody", entity, [this, &entity, componentSpacer, columnWidth, valueWidth](auto& component) {
		bool changed = false;

		changed |= EditorWidgets::drawCheckbox("Static", component.isStatic, columnWidth);

		static const char* responseItems[] = {"None", "Discrete"};
		int				   responseValue   = (int)component.responseType;
		if (EditorWidgets::drawCombo("Response Type", responseItems, 2, responseValue, columnWidth, columnWidth)) {
			component.responseType = (CollisionResponse)responseValue;
			changed				   = true;
		}

		changed |= EditorWidgets::drawVec2Control<glm::vec2>("Velocity", component.velocity, columnWidth, valueWidth);

		if (changed)
			ProjectManager::saveScene(m_scene);

		ImGui::Dummy(componentSpacer);
	});

	// -------------------------------------------------------------------------
	drawComponent<Component::Animations>("Animations", entity, [this, componentSpacer](auto& component) {
		if (ImGui::Button("Add Clip")) {
			if (!component.clips.contains("Unnamed animation")) {
				component.clips["Unnamed animation"] = AnimationClip{};
				m_selectedClip						 = "Unnamed animation";
				m_renameTarget						 = "Unnamed animation";
				m_selectionType						 = SelectionType::AnimationClip;
				m_focusRenameCursor					 = 2;
				ProjectManager::saveScene(m_scene);
			} else {
				AT_CORE_WARN("Clips cannot share the same name. Rename \"Unnamed animation\" first.");
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
				if (component.activeClip == clipName)
					component.activeClip = m_renamedClip;
				m_selectedClip = m_renamedClip;
				m_animationEditor.setClipName(m_renamedClip);
				m_renamedClip = {};
				ProjectManager::saveScene(m_scene);
				break;
			}

			if (!m_clipToDelete.empty()) {
				if (component.clips.contains(m_clipToDelete))
					component.clips.erase(m_clipToDelete);
				if (m_selectedClip == m_clipToDelete)
					m_selectedClip = {};
				m_clipToDelete = {};
				ProjectManager::saveScene(m_scene);
				break;
			}

			if (m_animationEditorShouldOpen) {
				m_animationEditorShouldOpen = false;
				m_animationEditor.open(&component, m_selectedClip, m_scene);
			}
		}
		ImGui::Dummy(componentSpacer);
	});

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
						component.name = name;
						ProjectManager::saveScene(m_scene);
					}
				}
				if (ImGui::Button("Create New Script")) {
					std::string defaultDir = ProjectManager::getActiveProject() ? ProjectManager::getActiveProject()->getData().src_directory : "";
					std::string path	   = Platform::saveFileDialog("h", defaultDir);
					if (!path.empty()) {
						std::string className = std::filesystem::path(path).stem().string();
						Behavior::generateNewScript(path);
						Platform::openFile(path);
						if (m_onNewScript)
							m_onNewScript(entity, className);
					}
				}
				ImGui::EndCombo();
			}
		} else {
			float changeButtonWidth = 62.0f * EditorWidgets::displayScale;
			ImGui::Text("Active Script: %s", component.instance->getTypeName().c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f * EditorWidgets::displayScale);
			if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0)))
				component.instance = nullptr;

			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 4.0f * EditorWidgets::displayScale));

			if (component.instance)
				drawScriptProperties(*component.instance);
		}
	});
}

void PropertiesPanel::drawClipLabel(std::string& clip, SelectionType type) {
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

	bool wasDimmed = false;
	if (clip == m_selectedClip) {
		tree_flags |= ImGuiTreeNodeFlags_Selected;
		m_selectionType = type;

		if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
			wasDimmed = true;
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, EditorWidgets::green);
			ImGui::PushStyleColor(ImGuiCol_Header, EditorWidgets::greenSub);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(EditorWidgets::greenSub.x, EditorWidgets::greenSub.y, EditorWidgets::greenSub.z, EditorWidgets::greenSub.w * 0.5f));
		}
	}

	bool opened = false;
	if (clip == m_renameTarget) {
		if (m_focusRenameCursor == 2)
			m_renameBuffer = clip;
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

	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseDoubleClicked(0)) {
			m_renameTarget		= clip;
			m_focusRenameCursor = 2;
		}
		if (ImGui::IsMouseClicked(0)) {
			m_selectedClip				= clip;
			m_selectionType				= type;
			m_animationEditorShouldOpen = (type == SelectionType::AnimationClip);
		}
	}

	if (ImGui::IsItemClicked())
		m_selectedClip = clip;

	if (clip == m_renameTarget && ImGui::IsItemDeactivated()) {
		m_renameTarget = {};
		ProjectManager::saveScene(m_scene);
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Rename")) {
			m_renameTarget		= clip;
			m_focusRenameCursor = 2;
		}
		if (ImGui::MenuItem("Delete"))
			m_clipToDelete = clip;
		ImGui::EndPopup();
	}

	if (opened)
		ImGui::TreePop();
}

void PropertiesPanel::drawScriptProperties(Behavior& behavior) {
	auto&		properties	= behavior.getProperties();
	const float columnWidth = 100.0f;

	for (auto& [name, property] : properties) {
		ImGui::PushID((name + property.name).c_str());

		drawScriptProperty(name.c_str(), property);

		ImGui::PopID();
	}
}

void PropertiesPanel::drawScriptProperty(const char* label, Behavior::Property& property) {
	bool changed = false;
	ImGui::SetNextItemWidth(-1);
	switch (property.type) {
		case BehaviorPropertyType::BOOL: {
			changed = EditorWidgets::drawCheckbox(label, *(bool*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::STRING: {
			changed = ImGui::InputText(label, (std::string*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::INT: {
			changed = EditorWidgets::drawIntControl(label, *(int*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::FLOAT: {
			changed = EditorWidgets::drawFloatControl(label, *(float*)property.valuePtr);
			break;
		}
		case BehaviorPropertyType::VEC2: {
			changed = EditorWidgets::drawVec2Control(label, *(glm::vec2*)property.valuePtr);
		} break;
		case BehaviorPropertyType::VEC3: {
			changed = EditorWidgets::drawVec3Control(label, *(glm::vec3*)property.valuePtr);
		} break;
		default:
			break;
	}
	if (changed)
		ProjectManager::saveScene(m_scene);
}

}  // namespace Atlas