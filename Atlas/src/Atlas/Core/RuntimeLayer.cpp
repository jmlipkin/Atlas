#include "atpch.h"
#include "RuntimeLayer.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/Renderer.h"

namespace Atlas {

RuntimeLayer::RuntimeLayer(bool preview) : Layer("Runtime Layer"), m_cameraController((float)Application::get().getWindow().getWidth() / (float)Application::get().getWindow().getHeight()), m_previewLayer(preview) {
	m_cameraController.setZoomLevel(25.0f);
}

void RuntimeLayer::setScene(std::shared_ptr<Scene> scene) {
	m_activeScene = scene;
	scene->setEventCallback([](Event& e) {
		Application::get().onEvent(e);
	});
}

void RuntimeLayer::onAttach() {
	std::shared_ptr<Scene> scene;
	if (m_previewLayer) {
		scene = ProjectManager::loadScene(Platform::getAppSupportPath() + "/preview.atscene");
	} else {
		scene = ProjectManager::loadBundledProject();
	}

	if (scene) {
		setScene(scene);
		if (!m_previewLayer) {
			ProjectManager::setActiveScene(scene);
		}
	} else {
		AT_CORE_ERROR("RuntimeLayer: Failed to load bundled project");
	}
}

void RuntimeLayer::onUpdate(DeltaTime dt) {
	Renderer::beginScene(m_cameraController.getCamera());
	if (m_activeScene != nullptr) m_activeScene->onUpdate(dt);
	Renderer::endScene();
}

}  // namespace Atlas