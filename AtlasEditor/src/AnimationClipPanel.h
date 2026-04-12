#pragma once

#include "Commands/CommandHistory.h"

#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Animation.h"

#include <imgui/imgui.h>

namespace Atlas {

class SubTexture;

class AnimationClipPanel {
  public:
	AnimationClipPanel(CommandHistory& commandHistory) : m_commandHistory(commandHistory) {}

	void open(Entity entity, const std::string& clipName, std::shared_ptr<Scene> scene);
	void close();

	void onImGuiRender();

	void syncClipName(const std::string& name) { m_clipName = name; }

  private:
	void drawClipLabel();
	bool drawFrameStrip(AnimationClip& clip);
	void drawFrame(SubTexture texture);

	void autoSave();
	void commitRename();

	Component::Animations* animations();

  private:
	std::shared_ptr<Scene> m_scene;
	Entity				   m_entity;

	CommandHistory& m_commandHistory;

	std::string m_clipName;
	bool		m_isOpen = false;

	std::string m_renameTarget;
	std::string m_renameBuffer;

	int m_focusRenameCursor = 0;

	std::string m_clipToDelete;
};

}  // namespace Atlas