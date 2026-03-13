#pragma once

#include "Version.h"

#include <string>
#include <vector>

namespace Atlas {
struct ProjectData {
	int atproj_version = 1;
	std::string atlas_version = AT_VERSION;
	std::string name;
	std::vector<std::string> scene_filepaths;
	std::string startup_scene;
};

class Project {
  public:
	Project(std::string project_path, ProjectData data) : m_path(project_path), m_data(data) {}

    std::string& getPath() { return m_path; }
    ProjectData& getData() { return m_data; }

  private:
	std::string m_path;
	ProjectData m_data;
};

}  // namespace Atlas