#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Project/Serializer.h"

#include "Version.h"

#include <memory>
#include <string>
#include <vector>

namespace Atlas {
struct ProjectData {
	inline static const int	  atproj_version = 1;
	inline static const char* atlas_version	 = AT_VERSION;

	std::string name;

	std::string src_directory = "";
	std::string scripts_target;

	std::vector<std::string> scene_filepaths;
	std::string				 startup_scene;
	std::string				 last_active_scene = "";

	int tileSize = 16;
	int pixelsPerUnit = 16;

	ProjectData(std::string Name = "UnnamedProject", std::string load_scene = "") : name(Name), startup_scene(load_scene), scripts_target(name + "Scripts") {
		if (!load_scene.empty()) {
			scene_filepaths.push_back(load_scene);
		}
	}
};

class Project {
  public:
	Project(std::string project_directory, ProjectData data) : m_directory(project_directory), m_data(data) {}

	void			   setName(const std::string& name) { m_data.name = name; }
	const std::string& getName() const { return m_data.name; }
	std::string&	   getName() { return m_data.name; }

	void setScripts(const std::vector<std::string>& scripts) { m_scripts = scripts; }

	std::vector<std::string>&		getScripts() { return m_scripts; }
	const std::vector<std::string>& getScripts() const { return m_scripts; }

	std::string& getDirectory() { return m_directory; }
	ProjectData& getData() { return m_data; }

  private:
	std::string m_directory;
	ProjectData m_data;

	std::vector<std::string> m_scripts;
};

class ProjectManager {
  public:
	static void					  saveScene(std::shared_ptr<Scene> scene, const std::string& explicitPath = "");
	static std::shared_ptr<Scene> loadScene(const std::string& filepath);

	static void attachScenetoProject(std::shared_ptr<Scene> scene);

	static void					  createNewProject(const std::string& filepath, const std::string& name);
	static std::shared_ptr<Scene> createNewScene(const std::string& filepath, const std::string& name);

	static void saveProjectAs(const std::string& filepath, const std::string& name);
	static void saveProject();

	static std::shared_ptr<Scene> loadBundledProject();

	// Returns the first active scene
	// TODO: determine internally which scene is the correct scene
	// based on run mode
	static std::shared_ptr<Scene> loadProject(const std::string& filepath, bool useStartupScene = false);
	static void					  closeProject(bool shouldSave);

	static void loadScriptLibrary();
	static void unloadScriptLibrary();

	static std::shared_ptr<Project> getActiveProject() { return s_activeProject; }
	static void						setActiveScene(std::shared_ptr<Scene> scene);

	static bool		   isDirty() { return s_isDirty; }
	static std::string toRelativePath(const std::string& absolutePath);
	static std::string toAbsolutePath(const std::string& relativePath);

  private:
	static void loadScriptManifest();

  private:
	static std::shared_ptr<Project> s_activeProject;
	static std::shared_ptr<Scene>	s_activeScene;
	static bool						s_isDirty;

	static AT_LIBRARY_HANDLE s_scriptLibHandle;
};

}  // namespace Atlas