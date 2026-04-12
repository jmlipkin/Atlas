#include "atpch.h"
#include "AnimationClipPanel.h"

#include "Commands/LambdaCommand.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture.h"
#include "Atlas/Project/Project.h"
#include "Atlas/ECS/Components/Animation.h"

#include "Atlas/ImGui/EditorWidgets.h"

#include <imgui/imgui.h>

namespace Atlas {

void AnimationClipPanel::open(Entity entity, const std::string& clipName, std::shared_ptr<Scene> scene) {
	m_scene	   = scene;
	m_entity   = entity;
	m_clipName = clipName;
	m_isOpen   = true;

	if (Component::Animations* anims = animations()) {
		if (anims->clips.contains(clipName))
			anims->activeClip = clipName;
	}
}

void AnimationClipPanel::close() {
	m_scene	   = nullptr;
	m_entity   = Entity{};
	m_clipName = {};
	m_isOpen   = false;
}

void AnimationClipPanel::onImGuiRender() {
	if (!m_isOpen) return;

	auto* anims = animations();
	if (!anims) {
		close();
		return;
	}
	if (!anims->clips.contains(m_clipName)) {
		close();
		return;
	}

	bool commandPushed = false;

	ImGui::Begin("Animation Clip Editor", &m_isOpen);
	EditorWidgets::DrawPanelAccentBar(EditorWidgets::PanelAccent::Purple);

	if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() &&
		!m_clipName.empty() && m_renameTarget.empty() &&
		ImGui::IsKeyPressed(ImGuiKey_Enter)) {
		m_renameTarget		= m_clipName;
		m_focusRenameCursor = 2;
	}

	drawClipLabel();

	// Re-fetch after potential rename/delete inside drawClipLabel.
	anims = animations();
	if (!anims || !anims->clips.contains(m_clipName)) {
		ImGui::End();
		if (anims) close();
		return;
	}

	AnimationClip& clip = anims->clips.at(m_clipName);

	std::string capturedClip = m_clipName;

	// Browse texture
	if (ImGui::Button("Browse##texture")) {
		std::string path = Platform::openFileDialog("png");
		if (!path.empty() && path != clip.texturePath) {
			std::string oldPath = clip.texturePath;
			std::string newPath = path;
			clip.texturePath	= newPath;
			m_commandHistory.push(std::make_unique<LambdaCommand>(
				"Change Clip Texture",
				[entity = m_entity, capturedClip, newPath]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).texturePath = newPath;
				},
				[entity = m_entity, capturedClip, oldPath]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).texturePath = oldPath;
				}));
			commandPushed = true;
		}
	}
	ImGui::SameLine();

	float playButtonWidth = 40;
	float padding		  = 4;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - playButtonWidth - padding);
	if (ImGui::Button(clip.playing ? "Stop" : "Play", ImVec2(playButtonWidth, 0))) {
		clip.playing = !clip.playing;
	}

	// Texture
	float		maxWidth	 = ImGui::GetContentRegionAvail().x * 0.4f;
	std::string relativePath = clip.texturePath.empty() ? "" : ProjectManager::toRelativePath(clip.texturePath);

	std::string displayPath = relativePath.empty() ? "No texture" : relativePath;

	if (!relativePath.empty() && ImGui::CalcTextSize(displayPath.c_str()).x > maxWidth) {
		std::string truncated = std::filesystem::path(relativePath).filename().string();
		while (!truncated.empty() &&
			   ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth) {
			truncated.pop_back();
		}
		displayPath = truncated + "...";
	}

	ImGui::Text("%s", displayPath.c_str());
	if (ImGui::IsItemHovered() && !relativePath.empty()) {
		ImGui::SetTooltip("%s", relativePath.c_str());
	}

	float columnWidth = 130.0f;
	float valueWidth  = 50.0f;

	// shouldLoop
	{
		static bool oldVal;
		auto		s = EditorWidgets::drawCheckbox("Should Loop", clip.shouldLoop, columnWidth);
		if (s.started) oldVal = !clip.shouldLoop;
		if (s.finished) {
			bool newVal		 = clip.shouldLoop;
			bool capturedOld = oldVal;
			m_commandHistory.push(std::make_unique<LambdaCommand>(
				"Toggle Loop",
				[entity = m_entity, capturedClip, newVal]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).shouldLoop = newVal;
				},
				[entity = m_entity, capturedClip, capturedOld]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).shouldLoop = capturedOld;
				}));
			commandPushed = true;
		}
	}

	// sizeInTiles
	{
		static glm::ivec2 oldVal;
		auto			  s = EditorWidgets::drawVec2Control<glm::ivec2>("Size", clip.sizeInTiles, columnWidth, valueWidth);
		if (s.started) oldVal = clip.sizeInTiles;
		if (s.finished && oldVal != clip.sizeInTiles) {
			glm::ivec2 newVal	   = clip.sizeInTiles;
			glm::ivec2 capturedOld = oldVal;
			m_commandHistory.push(std::make_unique<LambdaCommand>(
				"Resize Clip",
				[entity = m_entity, capturedClip, newVal]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).sizeInTiles = newVal;
				},
				[entity = m_entity, capturedClip, capturedOld]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).sizeInTiles = capturedOld;
				}));
			commandPushed = true;
		}
	}

	// frameRate
	{
		static float oldVal;
		auto		 s = EditorWidgets::drawFloatSlider("Frame Rate (fps)", clip.frameRate, columnWidth, valueWidth * 3.0f, 12.0f, 0.1f, 60.0f);
		if (s.started) oldVal = clip.frameRate;
		if (s.finished && oldVal != clip.frameRate) {
			float newVal	  = clip.frameRate;
			float capturedOld = oldVal;
			m_commandHistory.push(std::make_unique<LambdaCommand>(
				"Change Frame Rate",
				[entity = m_entity, capturedClip, newVal]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).frameRate = newVal;
				},
				[entity = m_entity, capturedClip, capturedOld]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.at(capturedClip).frameRate = capturedOld;
				}));
			commandPushed = true;
		}
	}

	ImGui::Separator();
	ImGui::Text("Frames");

	if (drawFrameStrip(clip))
		commandPushed = true;

	if (commandPushed) autoSave();

	ImGui::End();
}

void AnimationClipPanel::drawClipLabel() {
	ImGuiTreeNodeFlags tree_flags =
		ImGuiTreeNodeFlags_DrawLinesFull |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanFullWidth |
		ImGuiTreeNodeFlags_Selected;

	bool wasDimmed = false;
	if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
		wasDimmed = true;
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, EditorWidgets::green);
		ImGui::PushStyleColor(ImGuiCol_Header, EditorWidgets::greenSub);  // greenSub dimmed
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(EditorWidgets::greenSub.x, EditorWidgets::greenSub.y, EditorWidgets::greenSub.z, EditorWidgets::greenSub.w * 0.5f));
	}

	bool opened = false;
	if (m_clipName == m_renameTarget) {
		if (m_focusRenameCursor == 2) {
			m_renameBuffer = m_clipName;
		}
		if (m_focusRenameCursor > 0) {
			ImGui::SetKeyboardFocusHere();
			m_focusRenameCursor--;
		}
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		bool confirmed	 = ImGui::InputText("##rename", &m_renameBuffer,
											ImGuiInputTextFlags_AutoSelectAll |
												ImGuiInputTextFlags_EnterReturnsTrue);
		bool deactivated = ImGui::IsItemDeactivated();

		if (confirmed || deactivated)
			commitRename();
	} else {
		ImGui::PushID(m_clipName.c_str());
		opened = ImGui::TreeNodeEx("##clip", tree_flags, "%s", m_clipName.c_str());
		ImGui::PopID();
	}

	if (wasDimmed) ImGui::PopStyleColor(3);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
		m_renameTarget		= m_clipName;
		m_focusRenameCursor = 2;
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Rename")) {
			m_renameTarget		= m_clipName;
			m_focusRenameCursor = 2;
		}
		if (ImGui::MenuItem("Delete"))
			m_clipToDelete = m_clipName;
		ImGui::EndPopup();
	}

	if (opened) {
		ImGui::TreePop();
	}

	if (!m_clipToDelete.empty()) {
		auto* anims = animations();
		if (anims && anims->clips.contains(m_clipToDelete)) {
			AnimationClip deletedClip = anims->clips.at(m_clipToDelete);
			std::string	  deletedName = m_clipToDelete;
			m_commandHistory.push(std::make_unique<LambdaCommand>(
				"Delete Clip",
				[entity = m_entity, deletedName]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips.erase(deletedName);
				},
				[entity = m_entity, deletedName, deletedClip]() mutable {
					entity.refresh();
					entity.getComponent<Component::Animations>().clips[deletedName] = deletedClip;
				}));
			anims->clips.erase(m_clipToDelete);
			autoSave();
		}
		m_clipToDelete = {};
	}
}

void AnimationClipPanel::commitRename() {
	std::string oldName = m_clipName;
	std::string newName = m_renameBuffer;
	m_renameTarget		= {};
	ImGui::SetKeyboardFocusHere(-1);

	if (newName.empty() || newName == oldName) return;

	auto* anims = animations();
	if (!anims || !anims->clips.contains(oldName) || anims->clips.contains(newName)) return;

	// Apply immediately so the panel cursor is valid this frame.
	{
		auto node  = anims->clips.extract(oldName);
		node.key() = newName;
		anims->clips.insert(std::move(node));
		if (anims->activeClip == oldName) anims->activeClip = newName;
		m_clipName = newName;
	}

	m_commandHistory.push(std::make_unique<LambdaCommand>(
		"Rename Clip",
		[entity = m_entity, oldName, newName]() mutable {
			entity.refresh();
			auto& a	   = entity.getComponent<Component::Animations>();
			auto  node = a.clips.extract(oldName);
			node.key() = newName;
			a.clips.insert(std::move(node));
			if (a.activeClip == oldName) a.activeClip = newName;
		},
		[entity = m_entity, oldName, newName]() mutable {
			entity.refresh();
			auto& a	   = entity.getComponent<Component::Animations>();
			auto  node = a.clips.extract(newName);
			node.key() = oldName;
			a.clips.insert(std::move(node));
			if (a.activeClip == newName) a.activeClip = oldName;
		}));

	autoSave();
}

// ---------------------------------------------------------------------------
//  drawFrameStrip
// ---------------------------------------------------------------------------

bool AnimationClipPanel::drawFrameStrip(AnimationClip& clip) {
	bool commandPushed = false;

	float displayScale = EditorWidgets::displayScale;

	float cellWidth	   = 75;
	float imageWidth   = 37.5;
	float controlWidth = 75;

	float scrollbarHeight = ImGui::GetStyle().ScrollbarSize + 2.0f;
	float lineHeight	  = ImGui::GetFrameHeight();
	float controlHeight	  = lineHeight * 2 + GImGui->Style.ItemSpacing.y;

	const float stripPadX = 4.0f;
	const float stripPadY = 2.0f;

	float cellHeight  = imageWidth + (2 * displayScale) + controlHeight + stripPadY * 2;
	float stripHeight = cellHeight + scrollbarHeight + stripPadY * 2;

	ImVec4 frameBgColor	 = ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);
	int	   frameToDelete = -1;

	std::string capturedClip = m_clipName;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(stripPadX, stripPadY));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
	ImGui::BeginChild("##framestrip", ImVec2(0, stripHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);

	for (int i = 0; i <= (int)clip.frames.size(); i++) {
		ImGui::PushID(i);

		ImGui::InvisibleButton("##dragTarget", ImVec2(2.5 * displayScale, cellHeight));

		// Register as drop target
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FRAME_INDEX")) {
				int srcIndex = *(const int*)payload->Data;
				int dstIndex = i;

				if (srcIndex != dstIndex && srcIndex != dstIndex - 1) {
					if (srcIndex < dstIndex) dstIndex--;
					int capturedSrc = srcIndex;
					int capturedDst = dstIndex;
					// Apply immediately for responsiveness.
					auto frame = clip.frames[srcIndex];
					clip.frames.erase(clip.frames.begin() + srcIndex);
					clip.frames.insert(clip.frames.begin() + dstIndex, frame);
					m_commandHistory.push(std::make_unique<LambdaCommand>(
						"Reorder Frame",
						[entity = m_entity, capturedClip, capturedSrc, capturedDst]() mutable {
							entity.refresh();
							auto& frames = entity.getComponent<Component::Animations>().clips.at(capturedClip).frames;
							auto  f		 = frames[capturedSrc];
							frames.erase(frames.begin() + capturedSrc);
							frames.insert(frames.begin() + capturedDst, f);
						},
						[entity = m_entity, capturedClip, capturedSrc, capturedDst]() mutable {
							entity.refresh();
							auto& frames = entity.getComponent<Component::Animations>().clips.at(capturedClip).frames;
							auto  f		 = frames[capturedDst];
							frames.erase(frames.begin() + capturedDst);
							frames.insert(frames.begin() + capturedSrc, f);
						}));
					commandPushed = true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0)) {
			ImDrawList* dl	= ImGui::GetWindowDrawList();
			ImVec2		min = ImGui::GetItemRectMin();
			ImVec2		max = ImGui::GetItemRectMax();
			float		x	= (min.x + max.x) * 0.5f;
			dl->AddLine(
				ImVec2(x, min.y),
				ImVec2(x, max.y),
				ImGui::ColorConvertFloat4ToU32(EditorWidgets::steelBlue),
				1.0f);
		}

		if (i == (int)clip.frames.size()) {
			ImGui::SameLine();
			float buttonSize	= ImGui::GetFrameHeight();
			float buttonOffsetY = (cellHeight - buttonSize) * 0.5;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);
			if (ImGui::Button("+##frame", ImVec2(buttonSize, buttonSize))) {
				clip.frames.push_back({glm::ivec2(0, 0)});
				int newIndex = (int)clip.frames.size() - 1;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Add Frame",
					[entity = m_entity, capturedClip]() mutable {
						entity.refresh();
						entity.getComponent<Component::Animations>().clips.at(capturedClip).frames.push_back({glm::ivec2(0, 0)});
					},
					[entity = m_entity, capturedClip]() mutable {
						entity.refresh();
						auto& frames = entity.getComponent<Component::Animations>().clips.at(capturedClip).frames;
						if (!frames.empty()) frames.pop_back();
					}));
				commandPushed = true;
			}
			ImGui::PopID();
			break;
		}

		ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGui::GetFrameHeight() * 0.25f);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, frameBgColor);
		ImGui::BeginChild(("##cell" + std::to_string(i)).c_str(), ImVec2(cellWidth, cellHeight), false);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// Center the image
		float imageOffsetX = (cellWidth - imageWidth) * 0.5f;
		ImGui::SetCursorPosX(imageOffsetX);

		ImVec2 framePos = ImGui::GetCursorPos();

		ImGui::InvisibleButton("##drag", ImVec2(imageWidth, imageWidth));

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("FRAME_INDEX", &i, sizeof(int));

			float imageOffsetX = (cellWidth - imageWidth) * 0.5f;
			ImGui::SetCursorPosX(imageOffsetX);
			drawFrame({clip.texturePath, m_scene->getTileSize(), clip.frames[i].index, clip.sizeInTiles});
			ImGui::Dummy(ImVec2(0, 2.0 * displayScale));

			float controlOffsetX = (cellWidth - controlWidth) * 0.5f;
			ImGui::SetCursorPosX(controlOffsetX);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
			ImGui::BeginChild("##dragctrl", ImVec2(controlWidth, 0), false, ImGuiWindowFlags_NoScrollbar);
			// Drag preview — no command wiring.
			glm::ivec2 previewIndex = clip.frames[i].index;
			EditorWidgets::drawVec2Control("", previewIndex, 0, 50, 0, 0, true);
			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem("##framecontext")) {
			if (ImGui::MenuItem("Delete frame"))
				frameToDelete = i;
			ImGui::EndPopup();
		}

		ImGui::SetCursorPos(framePos);
		drawFrame({clip.texturePath, m_scene->getTileSize(), clip.frames[i].index, clip.sizeInTiles});
		ImGui::Dummy(ImVec2(0, 2 * displayScale));	// Spacer

		float controlOffsetX = (cellWidth - controlWidth) * 0.5f - 2;
		ImGui::SetCursorPosX(controlOffsetX);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
		ImGui::BeginChild("##ctrl", ImVec2(controlWidth, 0), false, ImGuiWindowFlags_NoScrollbar);

		// Frame index — undoable.
		{
			static glm::ivec2 oldVal;
			int				  capturedI = i;
			auto			  s			= EditorWidgets::drawVec2Control("", clip.frames[i].index, 0, 50, 0, 0, true);
			if (s.started) oldVal = clip.frames[i].index;
			if (s.finished && oldVal != clip.frames[i].index) {
				glm::ivec2 newVal	   = clip.frames[i].index;
				glm::ivec2 capturedOld = oldVal;
				m_commandHistory.push(std::make_unique<LambdaCommand>(
					"Change Frame Index",
					[entity = m_entity, capturedClip, capturedI, newVal]() mutable {
						entity.refresh();
						entity.getComponent<Component::Animations>().clips.at(capturedClip).frames[capturedI].index = newVal;
					},
					[entity = m_entity, capturedClip, capturedI, capturedOld]() mutable {
						entity.refresh();
						entity.getComponent<Component::Animations>().clips.at(capturedClip).frames[capturedI].index = capturedOld;
					}));
				commandPushed = true;
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::PopID();
	}

	ImGui::EndChild();

	if (frameToDelete >= 0) {
		AnimationFrame deletedFrame = clip.frames[frameToDelete];
		int			   deletedIndex = frameToDelete;
		clip.frames.erase(clip.frames.begin() + frameToDelete);
		m_commandHistory.push(std::make_unique<LambdaCommand>(
			"Delete Frame",
			[entity = m_entity, capturedClip, deletedIndex]() mutable {
				entity.refresh();
				auto& frames = entity.getComponent<Component::Animations>().clips.at(capturedClip).frames;
				if (deletedIndex < (int)frames.size())
					frames.erase(frames.begin() + deletedIndex);
			},
			[entity = m_entity, capturedClip, deletedIndex, deletedFrame]() mutable {
				entity.refresh();
				auto& frames = entity.getComponent<Component::Animations>().clips.at(capturedClip).frames;
				if (deletedIndex <= (int)frames.size())
					frames.insert(frames.begin() + deletedIndex, deletedFrame);
			}));
		commandPushed = true;
	}

	return commandPushed;
}

void AnimationClipPanel::drawFrame(SubTexture texture) {
	void*					data	  = AssetManager::get<Texture>(texture.getTexturePath())->getData();
	SubTextureSpecification specs	  = texture.getSpecs();
	ImVec2					max		  = ImVec2(37.5, 37.5);
	ImVec2					size	  = {specs.sizeInTiles.x * m_scene->getTileSize(), specs.sizeInTiles.y * m_scene->getTileSize()};
	float					targetAR  = size.x / size.y;
	float					currentAR = max.x / max.y;

	if (currentAR > targetAR) {
		size = ImVec2(max.y * targetAR, max.y);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (max.x - size.x) * 0.5f);
	} else {
		size = ImVec2(max.x, max.x / targetAR);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (max.y - size.y) * 0.5f);
	}

	ImGui::ImageWithBg(data, size, {specs.coordinates.top_left.x, specs.coordinates.top_left.y}, {specs.coordinates.bottom_right.x, specs.coordinates.bottom_right.y});
}

void AnimationClipPanel::autoSave() {
	ProjectManager::saveScene(m_scene);
}

Component::Animations* AnimationClipPanel::animations() {
	if (!m_entity) return nullptr;
	m_entity.refresh();
	if (!m_entity.hasComponent<Component::Animations>()) return nullptr;
	return &m_entity.getComponent<Component::Animations>();
}

}  // namespace Atlas