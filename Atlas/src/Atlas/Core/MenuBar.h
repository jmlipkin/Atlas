#pragma once

#include <memory>

namespace Atlas {

class Scene;
class MenuBar {
  public:
	using SceneLoadedCallback = std::function<void(std::shared_ptr<Scene>)>;
	using SceneSavedCallback = std::function<void()>;
	using SceneCreatedCallback = std::function<void(std::shared_ptr<Scene>)>;

	virtual ~MenuBar() = default;

	void setOnSceneLoaded(SceneLoadedCallback callback) { m_onSceneLoaded = callback; }
	void setOnSceneSaved(SceneSavedCallback callback) { m_onSceneSaved = callback; }
	void setOnNewScene(SceneCreatedCallback callback) { m_onNewScene = callback; }

	virtual void					generateMenuBar(const std::string& title) = 0;
	static std::shared_ptr<MenuBar> create();

  protected:
	SceneLoadedCallback m_onSceneLoaded;
	SceneSavedCallback m_onSceneSaved;
	SceneCreatedCallback m_onNewScene;
};

}  // namespace Atlas