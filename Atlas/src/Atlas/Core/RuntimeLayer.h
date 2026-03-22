#pragma once

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Layer.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Core/Time.h"

#include "Atlas/Events/Event.h"
#include "Atlas/Project/Project.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Renderer/OrthographicCameraController.h"

#include <memory>

namespace Atlas {

class RuntimeLayer : public Layer {
  public:
	RuntimeLayer(bool preview = false) : Layer("Runtime Layer"), m_cameraController((float)Application::get().getWindow().getWidth() / (float)Application::get().getWindow().getHeight()), m_previewLayer(preview) {
		m_cameraController.setZoomLevel(25.0f);
	}

	virtual void setScene(std::shared_ptr<Scene> scene) { m_activeScene = scene; }

	virtual void onAttach() override {
		std::shared_ptr<Scene> scene;
		if (m_previewLayer) {
			scene = ProjectManager::loadScene(Platform::getAppSupportPath() + "/preview.atscene");
		} else {
			scene = ProjectManager::loadBundledProject();
		}

		if (scene) {
			m_activeScene = scene;
			ProjectManager::setActiveScene(scene);
		} else {
			AT_CORE_ERROR("RuntimeLayer: Failed to load bundled project");
		}
	}

	virtual void onUpdate(DeltaTime dt) override {
		Renderer::beginScene(m_cameraController.getCamera());
		if (m_activeScene != nullptr) m_activeScene->onUpdate(dt);
		Renderer::endScene();
	}

	virtual void onDetach() override { m_activeScene = nullptr; }

	virtual void onEvent(Event& event) override {
		m_activeScene->dispatchEvent(event);
	}

  private:
	std::shared_ptr<Scene>		 m_activeScene;
	OrthographicCameraController m_cameraController;
	bool						 m_previewLayer;
};

}  // namespace Atlas