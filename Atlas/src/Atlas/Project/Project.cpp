#include "atpch.h"
#include "Project.h"

namespace Atlas {

std::shared_ptr<Project> ProjectManager::s_activeProject = nullptr;

void ProjectManager::createNewProject(const std::string& filepath, const std::string& name) {
	s_activeProject = std::make_shared<Project>(filepath, ProjectData{name});
	Serializer::serializeProject(s_activeProject);
}
void ProjectManager::loadProject(const std::string& filepath) {
	s_activeProject = std::make_shared<Project>(filepath, ProjectData{});
	Serializer::deserializeProject(s_activeProject);
}
void ProjectManager::saveProjectAs(const std::string& filepath, const std::string& name) {
	if (s_activeProject == nullptr) {
		AT_CORE_ERROR("Active project is not set! Cannot save project");
		return;
	}
	s_activeProject->getPath() = filepath;
	s_activeProject->setName(name);
	Serializer::serializeProject(s_activeProject);
}
void ProjectManager::saveProject() {
	if (s_activeProject == nullptr) {
		AT_CORE_ERROR("Active project is not set! Cannot save project");
		return;
	}
	Serializer::serializeProject(s_activeProject);
}

void ProjectManager::closeProject(bool shouldSave) {
	if (shouldSave) {
		saveProject();
	}
	s_activeProject = nullptr;
}

}  // namespace Atlas