#pragma once

#include <Atlas.h>
#include "Atlas/Renderer/OrthographicCameraController.h"
#include "Atlas/Scene/Scene.h"
#include "LoggingPanel.h"

namespace Atlas {
class EditorLayer : public Layer {
  public:
	EditorLayer();

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(DeltaTime dt) override;
	virtual void onEvent(Event& event) override;

	virtual void onImGuiRender() override;

  private:
	LoggingPanel logger;

	OrthographicCameraController m_cameraController;
	glm::vec2 m_viewportSize;

	std::vector<std::shared_ptr<Scene>> m_scenes;
};

}  // namespace Atlas