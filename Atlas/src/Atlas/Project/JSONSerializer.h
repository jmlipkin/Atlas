#pragma once

#include "Atlas/Project/Serializer.h"
#include "Atlas/Scene/Scene.h"

namespace Atlas {

class JSONSerializer : public SerializerAPI {
  public:
	virtual void serializeProject(const std::shared_ptr<Project>& project) override;
	virtual void deserializeProject(std::shared_ptr<Project> project) override;

	virtual void serializeScene(const std::shared_ptr<Scene>& scene) override;
	virtual void deserializeScene(std::shared_ptr<Scene> scene) override;

	virtual void loadScriptManifest(std::shared_ptr<Project> project) override;
};

}  // namespace Atlas