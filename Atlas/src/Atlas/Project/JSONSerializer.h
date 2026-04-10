#pragma once

#include "Atlas/Project/Serializer.h"
#include "Atlas/ECS/Entities/Entity.h"

#include <nlohmann/json.hpp>

namespace Atlas {

using json = nlohmann::ordered_json;

class Snapshot;

class JSONSerializer : public SerializerAPI {
  public:
	virtual void serializeProject(const std::shared_ptr<Project>& project) override;
	virtual void deserializeProject(std::shared_ptr<Project> project) override;

	virtual void serializeScene(const std::shared_ptr<Scene>& scene) override;
	virtual void deserializeScene(std::shared_ptr<Scene> scene) override;

	virtual std::unique_ptr<Snapshot> saveEntitySnapshot(Entity entity) override;

	virtual void recallEntitySnapshot(Entity& dst, const Snapshot& snapshot) override;

	virtual void serializeTileset(const std::shared_ptr<Tileset>& tileset) override;
	virtual void deserializeTileset(std::shared_ptr<Tileset> tileset) override;

	virtual void loadScriptManifest(std::shared_ptr<Project> project) override;

  private:
	virtual void serializeEntity(Entity entity, json& dst);
	virtual void deserializeEntity(Entity& dst, const json& src);
};

}  // namespace Atlas