#include "atpch.h"
#include "Project.h"
#include <filesystem>
#include <memory>

#include "Atlas/Project/Serializer.h"
#include "Atlas/Scene/Scene.h"

namespace Atlas {

std::shared_ptr<Project> ProjectManager::s_activeProject = nullptr;
std::shared_ptr<Scene>	 ProjectManager::s_activeScene	 = nullptr;
bool					 ProjectManager::s_isDirty		 = false;

void ProjectManager::saveScene(std::shared_ptr<Scene> scene, const std::string& explicitPath) {
	if (!explicitPath.empty()) {
		scene->getPath() = explicitPath;
	} else if (std::filesystem::path(scene->getPath()).is_relative()) {
		if (s_activeProject == nullptr) {
			AT_CORE_ERROR("Active project is not set! Cannot save scene without providing path");
			return;
		}
		scene->getPath() = std::filesystem::path(s_activeProject->getDirectory()).parent_path().string() + "/scenes/" + scene->getName() + ".atscene";
	}
	std::filesystem::create_directories(
		std::filesystem::path(scene->getPath()).parent_path());
	Serializer::serializeScene(scene);
}

std::shared_ptr<Scene> ProjectManager::loadScene(const std::string& filepath) {
	std::string			   name	 = std::filesystem::path(filepath).filename().stem().string();
	std::shared_ptr<Scene> scene = std::make_shared<Scene>(name);
	scene->getPath()			 = filepath;
	Serializer::deserializeScene(scene);

	s_activeScene = scene;
	return scene;
}

void ProjectManager::attachScenetoProject(std::shared_ptr<Scene> scene) {
	if (!s_activeProject) {
		AT_CORE_WARN("Cannot attach scene to project: no active project!");
		return;
	}

	std::string				  path	= toRelativePath(scene->getPath());
	std::vector<std::string>& paths = s_activeProject->getData().scene_filepaths;

	if (std::find(paths.begin(), paths.end(), path) == paths.end()) {
		paths.push_back(path);
	}
	s_isDirty = true;
}

void ProjectManager::createNewProject(const std::string& filepath, const std::string& name) {
	std::string absoluteProjectPath = (std::filesystem::path(filepath) / name).string();
	std::filesystem::create_directory(absoluteProjectPath);
	std::filesystem::create_directory(absoluteProjectPath + "/scenes");
	std::filesystem::create_directory(absoluteProjectPath + "/assets");
	s_activeProject = std::make_shared<Project>(absoluteProjectPath, ProjectData{name});
	Serializer::serializeProject(s_activeProject);
}

std::shared_ptr<Scene> ProjectManager::createNewScene(const std::string& filepath, const std::string& name) {
	std::shared_ptr<Scene> scene = std::make_shared<Scene>(name);
	scene->getPath()			 = filepath;

	if (s_activeProject) {
		attachScenetoProject(scene);
	}

	Serializer::serializeScene(scene);

	s_activeScene = scene;
	return scene;
}

std::shared_ptr<Scene> ProjectManager::loadProject(const std::string& filepath) {
	std::string directory = std::filesystem::path(filepath).parent_path().string();
	std::string name	  = std::filesystem::path(filepath).stem().string();
	s_activeProject		  = std::make_shared<Project>(directory, ProjectData{name});
	Serializer::deserializeProject(s_activeProject);
	if (!s_activeProject->getData().last_active_scene.empty()) {
		s_activeScene = loadScene(toAbsolutePath(s_activeProject->getData().last_active_scene));
	}
	return s_activeScene;
}

void ProjectManager::saveProjectAs(const std::string& filepath, const std::string& name) {
	if (s_activeProject == nullptr) {
		AT_CORE_ERROR("Active project is not set! Cannot save project");
		return;
	}
	s_activeProject->getDirectory() = filepath;
	s_activeProject->setName(name);
	Serializer::serializeProject(s_activeProject);
	s_isDirty = false;
}

void ProjectManager::saveProject() {
	if (s_activeProject == nullptr) {
		AT_CORE_ERROR("Active project is not set! Cannot save project");
		return;
	}
	Serializer::serializeProject(s_activeProject);

	if (s_activeScene && std::filesystem::path(s_activeScene->getPath()).is_absolute()) {
		for (auto& relativePath : s_activeProject->getData().scene_filepaths) {
			if (toAbsolutePath(relativePath) == s_activeScene->getPath()) {
				Serializer::serializeScene(s_activeScene);
				break;
			}
		}
	}

	s_isDirty = false;
}

void ProjectManager::closeProject(bool shouldSave) {
	if (shouldSave && s_activeProject) {
		saveProject();
	}
	s_activeProject = nullptr;
	s_activeScene	= nullptr;
}

void ProjectManager::setActiveScene(std::shared_ptr<Scene> scene) {
	s_activeScene = scene;
	if (s_activeProject && scene) {
		s_activeProject->getData().last_active_scene = toRelativePath(scene->getPath());
	}
	s_isDirty = true;
}

std::string ProjectManager::toRelativePath(const std::string& absolutePath) {
	AT_CORE_ASSERT(s_activeProject, "Active project not set! Cannot create relative path");
	return std::filesystem::relative(absolutePath, s_activeProject->getDirectory()).string();
}

std::string ProjectManager::toAbsolutePath(const std::string& relativePath) {
	AT_CORE_ASSERT(s_activeProject, "Active project not set! Cannot create absolute path");
	return (std::filesystem::path(s_activeProject->getDirectory()) / relativePath).string();
}

}  // namespace Atlas