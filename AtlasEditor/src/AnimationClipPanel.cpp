#include "atpch.h"
#include "AnimationClipPanel.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture.h"
#include "Atlas/Project/Project.h"
#include "Atlas/ECS/Components/Animation.h"

#include "Atlas/ImGui/EditorWidgets.h"

#include <imgui/imgui.h>

namespace Atlas {
void AnimationClipPanel::open(Component::Animations* animations, const std::string& clipName, std::shared_ptr<Scene> scene) {
	m_scene		 = scene;
	m_animations = animations;
	m_clipName	 = clipName;
	m_isOpen	 = true;

	if (m_animations->clips.contains(clipName)) {
		m_animations->activeClip = clipName;
	}
}

void AnimationClipPanel::close() {
	m_scene		 = nullptr;
	m_animations = nullptr;
	m_clipName	 = {};
	m_isOpen	 = false;
}

void AnimationClipPanel::onImGuiRender() {
	if (!m_isOpen || m_animations == nullptr) {
		return;
	}

	if (!m_animations->clips.contains(m_clipName)) {
		close();
		return;
	}

	ImGui::Begin("Animation Clip Editor", &m_isOpen);
	EditorWidgets::DrawPanelAccentBar(EditorWidgets::PanelAccent::Purple);

	if (m_justFinishedRename) {
		m_justFinishedRename = false;
	} else if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !m_clipName.empty() && m_renameTarget.empty() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
		m_renameTarget		= m_clipName;
		m_focusRenameCursor = 2;
	}

	drawClipLabel();
	AnimationClip& clip = m_animations->clips.at(m_clipName);

	bool changed = false;

	if (ImGui::Button("Browse##texture")) {
		std::string path = Platform::openFileDialog("png");
		if (!path.empty()) {
			clip.texturePath = path;
			changed			 = true;
		}
	}
	ImGui::SameLine();

	float playButtonWidth = 40 * EditorWidgets::displayScale;
	float padding		  = 4 * EditorWidgets::displayScale;
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
	changed |= EditorWidgets::drawCheckbox("Should Loop", clip.shouldLoop, columnWidth);
	changed |= EditorWidgets::drawVec2Control<glm::ivec2>("Size", clip.sizeInTiles, columnWidth, valueWidth);
	changed |= EditorWidgets::drawFloatSlider("Frame Rate (fps)", clip.frameRate, columnWidth, valueWidth * 3, 12.0f, 0.1f, 60.0f);

	ImGui::Separator();
	ImGui::Text("Frames");

	changed |= drawFrameStrip(clip);

	if (changed) autoSave();

	ImGui::End();
}

void AnimationClipPanel::drawClipLabel() {
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

	bool wasDimmed = false;
	tree_flags |= ImGuiTreeNodeFlags_Selected;

	bool panelFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	if (!panelFocused) {
		wasDimmed = true;
		// Dimmed selection color when panel is inactive
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
		if (ImGui::InputText("##rename", &m_renameBuffer, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
			m_renamedClip = m_renameBuffer;
			if (!m_renameTarget.empty() && !m_renamedClip.empty() && !m_animations->clips.contains(m_renamedClip)) {
				if (m_animations->clips.contains(m_clipName)) {
					if (m_animations->activeClip == m_clipName) {
						m_animations->activeClip = m_renamedClip;
					}
					auto node  = m_animations->clips.extract(m_clipName);
					node.key() = m_renamedClip;
					m_animations->clips.insert(std::move(node));
				}
			}
			m_clipName	   = m_renamedClip;
			m_renameTarget = {};
			ImGui::SetKeyboardFocusHere(-1);
			m_justFinishedRename = true;
			autoSave();
		}
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

	if (m_clipName == m_renameTarget && ImGui::IsItemDeactivated()) {
		if (!m_renameTarget.empty() && !m_renamedClip.empty() && !m_animations->clips.contains(m_renamedClip)) {
			if (m_animations->clips.contains(m_clipName)) {
				if (m_animations->activeClip == m_clipName) {
					m_animations->activeClip = m_renamedClip;
				}
				auto node  = m_animations->clips.extract(m_clipName);
				node.key() = m_renamedClip;
				m_animations->clips.insert(std::move(node));
			}
		}
		m_clipName	   = m_renamedClip;
		m_renameTarget = {};
		autoSave();
	}

	bool clipDeleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Rename")) {
			m_renameTarget		= m_clipName;
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
		m_clipToDelete = m_clipName;
	}
}

bool AnimationClipPanel::drawFrameStrip(AnimationClip& clip) {
	bool changed = false;

	float displayScale = EditorWidgets::displayScale;

	float cellWidth	   = 75;
	float imageWidth   = 37.5;
	float controlWidth = 75;

	float scrollbarHeight = ImGui::GetStyle().ScrollbarSize + 2.0f * displayScale;
	float lineHeight	  = ImGui::GetFrameHeight();
	float controlHeight	  = lineHeight * 2 + GImGui->Style.ItemSpacing.y;

	const float stripPadX = 4 * displayScale;
	const float stripPadY = 2 * displayScale;

	float cellHeight  = imageWidth + (2 * displayScale) + controlHeight + stripPadY * 2;
	float stripHeight = cellHeight + scrollbarHeight + stripPadY * 2;

	ImVec4 frameBgColor	 = ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);
	int	   frameToDelete = -1;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(stripPadX, stripPadY));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
	ImGui::BeginChild("##framestrip", ImVec2(0, stripHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);

	for (int i = 0; i <= clip.frames.size(); i++) {
		ImGui::PushID(i);

		ImGui::InvisibleButton("##dragTarget", ImVec2(2.5 * displayScale, cellHeight));

		// Register as drop target
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FRAME_INDEX")) {
				int srcIndex = *(const int*)payload->Data;
				int dstIndex = i;

				if (srcIndex != dstIndex && srcIndex != dstIndex - 1) {
					if (srcIndex < dstIndex) dstIndex--;
					auto frame = clip.frames[srcIndex];
					clip.frames.erase(clip.frames.begin() + srcIndex);
					clip.frames.insert(clip.frames.begin() + dstIndex, frame);
					changed = true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Draw insertion line when hovered during drag
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

		if (i == clip.frames.size()) {
			ImGui::SameLine();
			float buttonSize	= ImGui::GetFrameHeight();
			float buttonOffsetY = (cellHeight - buttonSize) * 0.5;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonOffsetY);
			if (ImGui::Button("+##frame", ImVec2(buttonSize, buttonSize))) {
				clip.frames.push_back({glm::ivec2(0, 0)});
				changed = true;
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
			EditorWidgets::drawVec2Control("", clip.frames[i].index, 0, 50, 0, 0, true);
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
		EditorWidgets::drawVec2Control("", clip.frames[i].index, 0, 50, 0, 0, true);
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::PopID();
	}

	ImGui::EndChild();

	if (frameToDelete >= 0) {
		clip.frames.erase(clip.frames.begin() + frameToDelete);
		changed = true;
	}

	return changed;
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
	if (m_animations == nullptr) {
		return;
	}
	ProjectManager::saveScene(m_scene);
}

}  // namespace Atlas