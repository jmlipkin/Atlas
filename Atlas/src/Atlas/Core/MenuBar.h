#pragma once

#include <memory>
#include <functional>

namespace Atlas {

class Project;
class Scene;
class MenuBar {
  public:
	using ProjectChangedCallback = std::function<void(std::string)>;
	using SceneLoadedCallback	 = std::function<void(std::shared_ptr<Scene>)>;
	using SceneSavedCallback	 = std::function<void()>;
	using SceneCreatedCallback	 = std::function<void(std::shared_ptr<Scene>)>;
	using ProjectClosedCallback	 = std::function<void()>;
	using SceneClosedCallback	 = std::function<void()>;

	using NewEntityCallback	   = std::function<void()>;
	using AddComponentCallback = std::function<void()>;

	using PreviewCallback = std::function<void()>;
	using BuildCallback	  = std::function<bool()>;

	using ValidationCallback = std::function<bool()>;

	using UndoCallback = std::function<void()>;
	using RedoCallback = std::function<void()>;

	virtual ~MenuBar() = default;

	// Takes path to project file
	void setOnProjectChanged(ProjectChangedCallback callback) { m_onProjectChanged = callback; }
	void setOnSceneLoaded(SceneLoadedCallback callback) { m_onSceneLoaded = callback; }
	void setOnSceneSaved(SceneSavedCallback callback) { m_onSceneSaved = callback; }
	void setOnNewScene(SceneCreatedCallback callback) { m_onNewScene = callback; }
	void setOnProjectClosed(ProjectClosedCallback callback) { m_onProjectClosed = callback; }
	void setOnSceneClosed(SceneClosedCallback callback) { m_onSceneClosed = callback; }

	void setOnNewEntity(NewEntityCallback callback) { m_onNewEntity = callback; }
	void setOnAddComponent(AddComponentCallback callback) { m_onAddComponent = callback; }

	void setOnPreview(PreviewCallback callback) { m_onPreview = callback; }
	void setOnBuild(BuildCallback callback) { m_onBuild = callback; }

	void setOnValidateSceneRequired(ValidationCallback callback) { m_onSceneValidation = callback; }
	void setOnValidateProjectRequired(ValidationCallback callback) { m_onProjectValidation = callback; }
	void setOnValidateBuildAvailable(ValidationCallback callback) { m_onBuildAvailable = callback; }

	void setOnUndo(UndoCallback callback) { m_onUndo = callback; }
	void setOnUndoAvailable(ValidationCallback callback) { m_onUndoAvailable = callback; }
	void setOnRedo(RedoCallback callback) { m_onRedo = callback; }
	void setOnRedoAvailable(ValidationCallback callback) { m_onRedoAvailable = callback; }

	virtual void updateUndoRedoMenuItems(const std::string& undoName, const std::string& redoName) = 0;

	virtual void					generateMenuBar(const std::string& title) = 0;
	static std::shared_ptr<MenuBar> create();

  protected:
	ProjectChangedCallback m_onProjectChanged;
	SceneLoadedCallback	   m_onSceneLoaded;
	SceneSavedCallback	   m_onSceneSaved;
	SceneCreatedCallback   m_onNewScene;
	ProjectClosedCallback  m_onProjectClosed;
	SceneClosedCallback	   m_onSceneClosed;

	NewEntityCallback	 m_onNewEntity;
	AddComponentCallback m_onAddComponent;

	PreviewCallback m_onPreview;
	BuildCallback	m_onBuild;

	UndoCallback m_onUndo;
	RedoCallback m_onRedo;

	ValidationCallback m_onSceneValidation;
	ValidationCallback m_onProjectValidation;
	ValidationCallback m_onBuildAvailable;

	ValidationCallback m_onUndoAvailable;
	ValidationCallback m_onRedoAvailable;
};

}  // namespace Atlas