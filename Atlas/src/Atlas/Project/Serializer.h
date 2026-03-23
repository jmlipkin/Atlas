#pragma once

#include "Atlas/Scene/Scene.h"

#include <glm/glm.hpp>

namespace Atlas {

class Project;

class SerializerAPI {
  public:
	virtual void serializeProject(const std::shared_ptr<Project>& project) = 0;
	virtual void deserializeProject(std::shared_ptr<Project> project)	   = 0;

	virtual void serializeScene(const std::shared_ptr<Scene>& scene) = 0;
	virtual void deserializeScene(std::shared_ptr<Scene> scene)		 = 0;

	virtual void loadScriptManifest(std::shared_ptr<Project> project) = 0;
};

class Serializer {
  public:
	virtual ~Serializer() = default;

	static void serializeProject(const std::shared_ptr<Project>& project) { s_serializerAPI->serializeProject(project); }
	static void deserializeProject(std::shared_ptr<Project> project) { s_serializerAPI->deserializeProject(project); }

	static void serializeScene(const std::shared_ptr<Scene>& scene) { s_serializerAPI->serializeScene(scene); }
	static void deserializeScene(std::shared_ptr<Scene> scene) { s_serializerAPI->deserializeScene(scene); }

	static void loadScriptManifest(std::shared_ptr<Project> project) { s_serializerAPI->loadScriptManifest(project); }

	static void init();

  private:
	static SerializerAPI* s_serializerAPI;
};

}  // namespace Atlas