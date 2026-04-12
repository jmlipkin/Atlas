#include "atpch.h"
#include "PropertiesPanel.h"

#include "Commands/SetPropertyCommand.h"
#include "Commands/LambdaCommand.h"

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

	// if (m_selectionType == SelectionType::AnimationClip)
	// 	m_animationEditor.syncClipName(m_selectedClip);

	ImGui::End();
}

void PropertiesPanel::drawComponents(Entity& entity) {
	auto& UUID = entity.getComponent<Component::UUID>().id;
	ImGui::Text("UUID: 0x%016llX", (uint64_t)UUID);

	drawTransformComponent(entity);
	drawSpriteComponent(entity);
	drawTilemapComponent(entity);
	drawColliderComponent(entity);
	drawRigidBodyComponent(entity);
	drawAnimationsComponent(entity);
	drawScriptComponent(entity);
}

void PropertiesPanel::drawTransformComponent(Entity& entity) {
	const float	 columnWidth	 = 100.0f;
	const float	 valueWidth		 = 85.0f;
	const ImVec2 componentSpacer = {0, 16.0f};

	drawComponent<Component::Transform>("Transform", entity, [this, &entity, componentSpacer, columnWidth, valueWidth](auto& component) {
		// Position XY
		{
			static glm::vec3 oldVal;  // TODO: per-entity capture
			glm::vec2		 pos = {component.position.x, component.position.y};
			auto			 s	 = EditorWidgets::drawVec2Control<glm::vec2>("Position", pos, columnWidth, valueWidth, 0, 0, true);
			if (s.changed) component.position = {pos, component.position.z};
			if (s.started) oldVal = component.position;
			if (s.finished && oldVal != component.position)
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::Transform, glm::vec3>>(
					"Move Entity", entity, &Component::Transform::position, oldVal, component.position));
		}

		// Origin
		if (entity.hasComponent<Component::Sprite>()) {
			static glm::vec2 oldVal;  // TODO: per-entity capture
			glm::vec2		 origin = System::Transformation::getCenter2D(entity);
			auto			 s		= EditorWidgets::drawVec2Control<glm::vec2>("Origin", origin, columnWidth, valueWidth);
			if (s.changed) System::Transformation::setCenter(entity, {origin.x, origin.y});
			if (s.started) oldVal = System::Transformation::getCenter2D(entity);
			if (s.finished && oldVal != origin) {
				glm::vec2 capturedOld = oldVal;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Move Origin",
					[entity, origin]() mutable {
						entity.refresh();
						System::Transformation::setCenter(entity, {origin.x, origin.y});
					},
					[entity, capturedOld]() mutable {
						entity.refresh();
						System::Transformation::setCenter(entity, {capturedOld.x, capturedOld.y});
					}));
			}
		}

		// Depth
		{
			static float oldVal;  // TODO: per-entity capture
			auto		 s = EditorWidgets::drawFloatControl("Depth", component.position.z, "D", columnWidth, valueWidth, 1.0f);
			if (s.started) oldVal = component.position.z;
			if (s.finished && oldVal != component.position.z) {
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::Transform, glm::vec3>>(
					"Change Depth", entity, &Component::Transform::position,
					glm::vec3{component.position.x, component.position.y, oldVal},
					component.position));
			}
			if (s.finished) ProjectManager::saveScene(m_scene);
		}

		ImGui::Dummy(componentSpacer);
	});
}

void PropertiesPanel::drawSpriteComponent(Entity& entity) {
	const float	 columnWidth	 = 100.0f;
	const float	 valueWidth		 = 85.0f;
	const ImVec2 componentSpacer = {0, 16.0f};
	const ImVec2 padding4		 = {0, 4.0f};

	drawComponent<Component::Sprite>("Sprite", entity, [this, &entity, componentSpacer, padding4, columnWidth, valueWidth](auto& component) {
		SubTextureSpecification& specs	  = component.specs;
		const std::string&		 filepath = ProjectManager::toRelativePath(component.texturePath);

		float changeButtonWidth = 62.0f * EditorWidgets::displayScale;
		float maxWidth			= ImGui::GetContentRegionAvail().x - changeButtonWidth - 8.0f;

		std::string displayPath = filepath.empty() ? "No texture" : filepath;
		if (!filepath.empty() && ImGui::CalcTextSize(filepath.c_str()).x > maxWidth) {
			std::string truncated = std::filesystem::path(filepath).filename().string();
			while (!truncated.empty() && ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth)
				truncated.pop_back();
			displayPath = truncated + "...";
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", displayPath.c_str());
		if (ImGui::IsItemHovered() && !filepath.empty())
			ImGui::SetTooltip("%s", filepath.c_str());

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f * EditorWidgets::displayScale);
		if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
			std::string oldPath = component.texturePath;
			std::string newPath = Platform::openFileDialog("png");
			if (!newPath.empty() && newPath != oldPath) {
				component.texturePath = newPath;
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::Sprite, std::string>>(
					"Change Texture", entity, &Component::Sprite::texturePath, oldPath, newPath));
				ProjectManager::saveScene(m_scene);
			}
		}

		ImGui::Dummy(padding4);
		bool saveNeeded = false;

		// Size
		{
			static glm::vec2 oldVal;  // TODO: per-entity capture
			auto			 s = EditorWidgets::drawVec2Control<glm::vec2>("Size", specs.sizeInTiles, columnWidth, valueWidth, 1, 1);
			if (s.started) oldVal = specs.sizeInTiles;
			if (s.changed) component.recalculateCoordinates();
			if (s.finished && oldVal != specs.sizeInTiles) {
				glm::vec2 newVal	  = specs.sizeInTiles;
				glm::vec2 capturedOld = oldVal;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Resize Sprite",
					[entity, newVal]() mutable {
						entity.refresh();
						auto& s				= entity.getComponent<Component::Sprite>();
						s.specs.sizeInTiles = newVal;
						s.recalculateCoordinates();
					},
					[entity, capturedOld]() mutable {
						entity.refresh();
						auto& s				= entity.getComponent<Component::Sprite>();
						s.specs.sizeInTiles = capturedOld;
						s.recalculateCoordinates();
					}));
				saveNeeded = true;
			}
		}

		// Index
		{
			static glm::ivec2 oldVal;  // TODO: per-entity capture
			auto			  s = EditorWidgets::drawVec2Control<glm::ivec2>("Index", specs.index, columnWidth, valueWidth);
			if (s.started) oldVal = specs.index;
			if (s.changed) component.recalculateCoordinates();
			if (s.finished && oldVal != specs.index) {
				glm::ivec2 newVal	   = specs.index;
				glm::ivec2 capturedOld = oldVal;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Change Sprite Index",
					[entity, newVal]() mutable {
						entity.refresh();
						auto& s		  = entity.getComponent<Component::Sprite>();
						s.specs.index = newVal;
						s.recalculateCoordinates();
					},
					[entity, capturedOld]() mutable {
						entity.refresh();
						auto& s		  = entity.getComponent<Component::Sprite>();
						s.specs.index = capturedOld;
						s.recalculateCoordinates();
					}));
				saveNeeded = true;
			}
		}

		if (saveNeeded) ProjectManager::saveScene(m_scene);
		ImGui::Dummy(componentSpacer);
	});
}

void PropertiesPanel::drawTilemapComponent(Entity& entity) {
	const float	 columnWidth	 = 100.0f;
	const float	 valueWidth		 = 85.0f;
	const ImVec2 componentSpacer = {0, 16.0f};
	const ImVec2 padding4		 = {0, 4.0f};

	drawComponent<Component::Tilemap>("Tile Map", entity, [this, &entity, componentSpacer, padding4, columnWidth, valueWidth](auto& component) {
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
					if (ImGui::Selectable(tileset.c_str()))
						AT_CORE_WARN("TODO: Tilemap assign tileset");
				}
				if (ImGui::Button("Create New Tileset")) {
					std::string defaultDir = ProjectManager::getActiveProject() ? ProjectManager::getActiveProject()->getData().src_directory : "";
					std::string path	   = Platform::saveFileDialog("attileset", defaultDir);
					if (!path.empty()) {
						ProjectManager::createTileset(path, std::filesystem::path(path).stem().string());
						AT_CORE_WARN("TODO: Create new tileset command");
						ProjectManager::saveScene(m_scene);
					}
				}
				ImGui::EndCombo();
			}
		} else {
			float changeButtonWidth = 62.0f * EditorWidgets::displayScale;
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Tileset: %s", component.tileset.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f * EditorWidgets::displayScale);
			if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::Tilemap, std::string>>(
					"Clear Tileset", entity, &Component::Tilemap::tileset, component.tileset, std::string{}));
				component.tileset	 = {};
				m_firstOpenedTilemap = true;
				ProjectManager::saveScene(m_scene);
			}

			ImGui::Separator();
			ImGui::Dummy(padding4);

			// Resize
			{
				static glm::ivec2		oldSize;  // TODO: per-entity capture
				static std::vector<int> oldGrid;
				glm::ivec2				size = component.size;
				auto					s	 = EditorWidgets::drawVec2Control<glm::ivec2>("Size", size, columnWidth, valueWidth);
				if (s.started) {
					oldSize = component.size;
					oldGrid = component.grid;
				}
				if (s.changed && size.x >= 1 && size.y >= 1) component.resize(size);
				if (s.finished && oldSize != component.size) {
					glm::ivec2		 newSize	  = component.size;
					std::vector<int> newGrid	  = component.grid;
					glm::ivec2		 capturedSize = oldSize;
					std::vector<int> capturedGrid = oldGrid;
					m_commandHistory.push(std::make_unique<LambdaCommand>(
						"Resize Tilemap",
						[entity, newSize, newGrid]() mutable {
							entity.refresh();
							auto& t = entity.getComponent<Component::Tilemap>();
							t.size	= newSize;
							t.grid	= newGrid;
						},
						[entity, capturedSize, capturedGrid]() mutable {
							entity.refresh();
							auto& t = entity.getComponent<Component::Tilemap>();
							t.size	= capturedSize;
							t.grid	= capturedGrid;
						}));
					ProjectManager::saveScene(m_scene);
				}
			}

			// Debug Overlay
			{
				static bool oldVal;	 // TODO: per-entity capture
				auto		s = EditorWidgets::drawCheckbox("Debug Overlay", component.showOverlay, columnWidth);
				if (s.started) oldVal = !component.showOverlay;	 // capture pre-mutation value
				if (s.finished) {
					m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::Tilemap, bool>>(
						"Toggle Debug Overlay", entity, &Component::Tilemap::showOverlay,
						oldVal, component.showOverlay));
					ProjectManager::saveScene(m_scene);
				}
			}

			ImGui::Dummy(padding4);
			if (ImGui::Button("Open Tilemap Editor"))
				m_tilemapEditorPanel.open(&component, m_scene, &entity);
		}

		ImGui::Dummy(componentSpacer);
	});
}

void PropertiesPanel::drawColliderComponent(Entity& entity) {
	const float	 columnWidth	 = 100.0f;
	const float	 valueWidth		 = 85.0f;
	const ImVec2 componentSpacer = {0, 16.0f};

	drawComponent<Component::Collider>("Collider", entity, [this, &entity, componentSpacer, columnWidth, valueWidth](auto& component) {
		bool saveNeeded = false;

		EditorWidgets::drawCheckbox("Show Debug", m_showCollider, columnWidth);

		// isTrigger
		{
			static bool oldVal;	 // TODO: per-entity capture
			auto		s = EditorWidgets::drawCheckbox("Trigger", component.isTrigger, columnWidth);
			if (s.started) oldVal = !component.isTrigger;  // capture pre-mutation value
			if (s.finished) {
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::Collider, bool>>(
					"Toggle Trigger", entity, &Component::Collider::isTrigger,
					oldVal, component.isTrigger));
				saveNeeded = true;
			}
		}

		// Shape
		{
			static const char* shapeItems[] = {"AABB", "Circle"};
			int				   shapeValue	= (int)component.shape;
			auto			   s			= EditorWidgets::drawCombo("Shape", shapeItems, 2, shapeValue, columnWidth, columnWidth);
			if (s.finished) {
				ColliderShape oldShape = component.shape;
				ColliderShape newShape = (ColliderShape)shapeValue;
				// Apply immediately so the size field below reflects the new shape this frame.
				component.shape = newShape;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Change Collider Shape",
					[entity, newShape]() mutable {
						entity.refresh();
						entity.getComponent<Component::Collider>().shape = newShape;
					},
					[entity, oldShape]() mutable {
						entity.refresh();
						entity.getComponent<Component::Collider>().shape = oldShape;
					}));
				saveNeeded = true;
			}
		}

		// Offset
		{
			static glm::vec2 oldVal;  // TODO: per-entity capture
			auto			 s = EditorWidgets::drawVec2Control<glm::vec2>("Offset", component.offset, columnWidth, valueWidth);
			if (s.started) oldVal = component.offset;
			if (s.finished && oldVal != component.offset) {
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::Collider, glm::vec2>>(
					"Move Collider", entity, &Component::Collider::offset, oldVal, component.offset));
				saveNeeded = true;
			}
		}

		if (component.shape == ColliderShape::AABB) {
			static glm::vec2 oldVal;  // TODO: per-entity capture
			auto			 s = EditorWidgets::drawVec2Control<glm::vec2>("Size", component.size.AABB, columnWidth, valueWidth);
			if (s.started) oldVal = component.size.AABB;
			if (s.finished && oldVal != component.size.AABB) {
				glm::vec2 newVal	  = component.size.AABB;
				glm::vec2 capturedOld = oldVal;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Resize Collider",
					[entity, newVal]() mutable {
						entity.refresh();
						entity.getComponent<Component::Collider>().size.AABB = newVal;
					},
					[entity, capturedOld]() mutable {
						entity.refresh();
						entity.getComponent<Component::Collider>().size.AABB = capturedOld;
					}));
				saveNeeded = true;
			}
		} else {
			static float oldVal;  // TODO: per-entity capture
			auto		 s = EditorWidgets::drawFloatControl("Radius", component.size.radius, "R", columnWidth, valueWidth, 0.1f);
			if (s.started) oldVal = component.size.radius;
			if (s.finished && oldVal != component.size.radius) {
				float newVal	  = component.size.radius;
				float capturedOld = oldVal;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Resize Collider",
					[entity, newVal]() mutable {
						entity.refresh();
						entity.getComponent<Component::Collider>().size.radius = newVal;
					},
					[entity, capturedOld]() mutable {
						entity.refresh();
						entity.getComponent<Component::Collider>().size.radius = capturedOld;
					}));
				saveNeeded = true;
			}
		}

		if (saveNeeded) ProjectManager::saveScene(m_scene);
		ImGui::Dummy(componentSpacer);
	});
}

void PropertiesPanel::drawRigidBodyComponent(Entity& entity) {
	const float	 columnWidth	 = 100.0f;
	const float	 valueWidth		 = 85.0f;
	const ImVec2 componentSpacer = {0, 16.0f};

	drawComponent<Component::RigidBody>("RigidBody", entity, [this, &entity, componentSpacer, columnWidth, valueWidth](auto& component) {
		bool saveNeeded = false;

		// isStatic
		{
			static bool oldVal;	 // TODO: per-entity capture
			auto		s = EditorWidgets::drawCheckbox("Static", component.isStatic, columnWidth);
			if (s.started) oldVal = !component.isStatic;  // capture pre-mutation value
			if (s.finished) {
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::RigidBody, bool>>(
					"Toggle Static", entity, &Component::RigidBody::isStatic,
					oldVal, component.isStatic));
				saveNeeded = true;
			}
		}

		// Response Type
		{
			static const char* responseItems[] = {"None", "Discrete"};
			int				   responseValue   = (int)component.responseType;
			auto			   s			   = EditorWidgets::drawCombo("Response Type", responseItems, 2, responseValue, columnWidth, columnWidth);
			if (s.finished) {
				CollisionResponse oldVal = component.responseType;
				CollisionResponse newVal = (CollisionResponse)responseValue;
				component.responseType	 = newVal;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Change Response Type",
					[entity, newVal]() mutable {
						entity.refresh();
						entity.getComponent<Component::RigidBody>().responseType = newVal;
					},
					[entity, oldVal]() mutable {
						entity.refresh();
						entity.getComponent<Component::RigidBody>().responseType = oldVal;
					}));
				saveNeeded = true;
			}
		}

		// Velocity
		{
			static glm::vec2 oldVal;  // TODO: per-entity capture
			auto			 s = EditorWidgets::drawVec2Control<glm::vec2>("Velocity", component.velocity, columnWidth, valueWidth);
			if (s.started) oldVal = component.velocity;
			if (s.finished && oldVal != component.velocity) {
				m_commandHistory.push(std::make_unique<SetPropertyCommand<Component::RigidBody, glm::vec2>>(
					"Set Velocity", entity, &Component::RigidBody::velocity, oldVal, component.velocity));
				saveNeeded = true;
			}
		}

		if (saveNeeded) ProjectManager::saveScene(m_scene);
		ImGui::Dummy(componentSpacer);
	});
}

void PropertiesPanel::drawAnimationsComponent(Entity& entity) {
	const ImVec2 componentSpacer = {0, 16.0f};

	drawComponent<Component::Animations>("Animations", entity, [this, &entity, componentSpacer](auto& component) {
		if (ImGui::Button("Add Clip")) {
			if (!component.clips.contains("Unnamed animation")) {
				component.clips["Unnamed animation"] = AnimationClip{};
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Add Clip",
					[entity]() mutable {
						entity.refresh();
						entity.getComponent<Component::Animations>().clips["Unnamed animation"] = AnimationClip{};
					},
					[entity]() mutable {
						entity.refresh();
						entity.getComponent<Component::Animations>().clips.erase("Unnamed animation");
					}));
				m_selectedClip		= "Unnamed animation";
				m_renameTarget		= "Unnamed animation";
				m_selectionType		= SelectionType::AnimationClip;
				m_focusRenameCursor = 2;
				ProjectManager::saveScene(m_scene);
			} else {
				AT_CORE_WARN("Clips cannot share the same name. Rename \"Unnamed animation\" first.");
			}
		}

		if (component.clips.empty())
			ImGui::Text("No clips exist!");

		for (auto& [clipName, clip] : component.clips) {
			std::string editableName = clipName;
			drawClipLabel(editableName, SelectionType::AnimationClip);

			if (!m_renamedClip.empty() && clipName == m_selectedClip && !component.clips.contains(m_renamedClip)) {
				std::string oldName = clipName;
				std::string newName = m_renamedClip;
				auto		node	= component.clips.extract(clipName);
				node.key()			= newName;
				component.clips.insert(std::move(node));
				if (component.activeClip == oldName)
					component.activeClip = newName;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Rename Clip",
					[entity, oldName, newName]() mutable {
						entity.refresh();
						auto& clips = entity.getComponent<Component::Animations>().clips;
						auto  node	= clips.extract(oldName);
						node.key()	= newName;
						clips.insert(std::move(node));
					},
					[entity, oldName, newName]() mutable {
						entity.refresh();
						auto& clips = entity.getComponent<Component::Animations>().clips;
						auto  node	= clips.extract(newName);
						node.key()	= oldName;
						clips.insert(std::move(node));
					}));
				m_selectedClip = newName;
				m_animationEditor.syncClipName(newName);
				m_renamedClip = {};
				ProjectManager::saveScene(m_scene);
				break;
			}

			if (!m_clipToDelete.empty()) {
				if (component.clips.contains(m_clipToDelete)) {
					AnimationClip deletedClip = component.clips.at(m_clipToDelete);
					std::string	  deletedName = m_clipToDelete;
					m_commandHistory.push(std::make_unique<LambdaCommand>(
						"Delete Clip",
						[entity, deletedName]() mutable {
							entity.refresh();
							entity.getComponent<Component::Animations>().clips.erase(deletedName);
						},
						[entity, deletedName, deletedClip]() mutable {
							entity.refresh();
							entity.getComponent<Component::Animations>().clips[deletedName] = deletedClip;
						}));
					component.clips.erase(m_clipToDelete);
				}
				if (m_selectedClip == m_clipToDelete)
					m_selectedClip = {};
				m_clipToDelete = {};
				ProjectManager::saveScene(m_scene);
				break;
			}

			if (m_animationEditorShouldOpen) {
				m_animationEditorShouldOpen = false;
				m_animationEditor.open(entity, m_selectedClip, m_scene);
			}
		}
		ImGui::Dummy(componentSpacer);
	});
}

void PropertiesPanel::drawScriptComponent(Entity& entity) {
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
						m_commandHistory.push(std::make_unique<LambdaCommand>(
							"Assign Script",
							[entity, name]() mutable {
								entity.refresh();
								auto& s	   = entity.getComponent<Component::Script>();
								s.instance = ScriptRegistry::create(name);
								s.priority = ScriptRegistry::getPriority(name);
								s.instance->setEntity(entity);
								s.instance->onCreate();
								s.instance->exposeProperties();
								s.name = name;
							},
							[entity]() mutable {
								entity.refresh();
								entity.getComponent<Component::Script>().instance = nullptr;
							}));
						ProjectManager::saveScene(m_scene);
					}
				}
				if (ImGui::Button("Create New Script")) {
					std::string defaultDir = ProjectManager::getActiveProject() ? ProjectManager::getActiveProject()->getData().src_directory : "";
					std::string path	   = Platform::saveFileDialog("h", defaultDir);
					if (!path.empty()) {
						Behavior::generateNewScript(path);
						Platform::openFile(path);
						if (m_onNewScript)
							m_onNewScript(entity, std::filesystem::path(path).stem().string());
					}
				}
				ImGui::EndCombo();
			}
		} else {
			float changeButtonWidth = 62.0f * EditorWidgets::displayScale;
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Active Script: %s", component.instance->getTypeName().c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f * EditorWidgets::displayScale);
			if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
				std::string oldName = component.name;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Clear Script",
					[entity]() mutable {
						entity.refresh();
						entity.getComponent<Component::Script>().instance = nullptr;
					},
					[entity, oldName]() mutable {
						entity.refresh();
						auto& s	   = entity.getComponent<Component::Script>();
						s.instance = ScriptRegistry::create(oldName);
						if (s.instance) {
							s.priority = ScriptRegistry::getPriority(oldName);
							s.instance->setEntity(entity);
							s.instance->onCreate();
							s.instance->exposeProperties();
							s.name = oldName;
						}
					}));
				component.instance = nullptr;
				ProjectManager::saveScene(m_scene);
			}

			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 4.0f * EditorWidgets::displayScale));

			if (component.instance)
				drawScriptProperties(*component.instance);	// TODO: undo for script properties
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
	auto& properties = behavior.getProperties();
	for (auto& [name, property] : properties) {
		ImGui::PushID((name + property.name).c_str());
		drawScriptProperty(name.c_str(), property);
		ImGui::PopID();
	}
}

void PropertiesPanel::drawScriptProperty(const char* label, Behavior::Property& property) {
	WidgetState state;

	const float columnWidth = 100.0f;
	const float valueWidth	= 85.0f;
	switch (property.type) {
		case BehaviorPropertyType::BOOL:
			state = EditorWidgets::drawCheckbox(label, *(bool*)property.valuePtr, columnWidth);
			break;
		case BehaviorPropertyType::STRING:
			state.changed = ImGui::InputText(label, (std::string*)property.valuePtr);
			break;
		case BehaviorPropertyType::INT:
			state = EditorWidgets::drawIntControl(label, *(int*)property.valuePtr, "R", columnWidth, valueWidth);
			break;
		case BehaviorPropertyType::FLOAT:
			state = EditorWidgets::drawFloatControl(label, *(float*)property.valuePtr, "R", columnWidth, valueWidth);
			break;
		case BehaviorPropertyType::VEC2:
			state = EditorWidgets::drawVec2Control(label, *(glm::vec2*)property.valuePtr, columnWidth, valueWidth);
			break;
		case BehaviorPropertyType::VEC3:
			state = EditorWidgets::drawVec3Control(label, *(glm::vec3*)property.valuePtr, columnWidth, valueWidth);
			break;
		default:
			break;
	}
	if (state.changed)
		ProjectManager::saveScene(m_scene);
}

}  // namespace Atlas