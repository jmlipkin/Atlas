#pragma once

#include "Atlas/Core/Layer.h"
#include "Atlas/Events/Event.h"

#include "Atlas/Renderer/OrthographicCameraController.h"
#include "Atlas/Scene/Scene.h"

namespace Atlas {

class RuntimeLayer : public Layer {
  public:
	RuntimeLayer(bool preview = false);

	virtual void setScene(std::shared_ptr<Scene> scene);

	virtual void onAttach() override;
	virtual void onUpdate(DeltaTime dt) override;
	virtual void onDetach() override { m_activeScene = nullptr; }

	virtual void onEvent(Event& event) override {
		m_cameraController.onEvent(event);
		m_activeScene->onEvent(event);
	}

  private:
	std::shared_ptr<Scene>		 m_activeScene;
	OrthographicCameraController m_cameraController;
	bool						 m_previewLayer;
};

}  // namespace Atlas