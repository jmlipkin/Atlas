#pragma once

#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Components/Animation.h"

#include <imgui/imgui.h>

namespace Atlas {

class SubTexture;

class AnimationClipPanel {
  public:
	void setScene(std::shared_ptr<Scene> scene) { m_scene = scene; }

	bool		isClipNameChanged() const { return m_justFinishedRename; }
	std::string getClipName() const { return m_clipName; }
	void		setClipName(std::string clip) { m_clipName = clip; }

	void open(Component::Animations* animations, const std::string& clipName, std::shared_ptr<Scene> scene);
	void close();

	void onImGuiRender();

  private:
	void drawClipLabel();
	bool drawFrameStrip(AnimationClip& clip);
	void drawFrame(SubTexture texture);
	void autoSave();

  private:
	std::shared_ptr<Scene> m_scene;
	Component::Animations* m_animations;

	std::string m_clipName;
	bool		m_isOpen = false;

	std::string m_renameTarget;
	std::string m_renameBuffer;
	std::string m_renamedClip;
	int			m_focusRenameCursor;
	bool		m_justFinishedRename = false;

	std::string m_clipToDelete;
};

}  // namespace Atlas