#pragma once

#include "Atlas/Renderer/Tileset.h"
#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Entities/Entity.h"

#include "Atlas/Project/Snapshot.h"
#include "Atlas/Project/JSONSnapshot.h"
#include "Atlas/Project/ComponentSerializer.h"

#include <glm/glm.hpp>
#include <memory>

namespace Atlas {

class Project;

class SerializerAPI {
  public:
	virtual ~SerializerAPI() = default;

	virtual void serializeProject(const std::shared_ptr<Project>& project) = 0;
	virtual void deserializeProject(std::shared_ptr<Project> project)	   = 0;

	virtual void serializeScene(const std::shared_ptr<Scene>& scene) = 0;
	virtual void deserializeScene(std::shared_ptr<Scene> scene)		 = 0;

	virtual std::unique_ptr<Snapshot> saveEntitySnapshot(Entity entity) = 0;
	virtual void recallEntitySnapshot(Entity& dst, const Snapshot& snapshot) = 0;

	virtual void serializeTileset(const std::shared_ptr<Tileset>& tileset) = 0;
	virtual void deserializeTileset(std::shared_ptr<Tileset> tileset)	   = 0;

	virtual void loadScriptManifest(std::shared_ptr<Project> project) = 0;
};

class Serializer {
  public:
	virtual ~Serializer() = default;

	static void serializeProject(const std::shared_ptr<Project>& project) { s_serializerAPI->serializeProject(project); }
	static void deserializeProject(std::shared_ptr<Project> project) { s_serializerAPI->deserializeProject(project); }

	static void serializeScene(const std::shared_ptr<Scene>& scene) { s_serializerAPI->serializeScene(scene); }
	static void deserializeScene(std::shared_ptr<Scene> scene) { s_serializerAPI->deserializeScene(scene); }

	static std::unique_ptr<Snapshot> saveEntitySnapshot(Entity entity) { return s_serializerAPI->saveEntitySnapshot(entity); }
	static void recallEntitySnapshot(Entity& dst, const Snapshot& snapshot) { s_serializerAPI->recallEntitySnapshot(dst, snapshot); }

	template <typename T>
	static std::unique_ptr<Snapshot> saveComponentSnapshot(Entity entity) {
		auto snapshot = std::make_unique<JSONSnapshot>();
		ComponentSerializer<T>::serialize(entity, snapshot->data);
		return snapshot;
	}
	template <typename T>
	static void recallComponentSnapshot(Entity& dst, const Snapshot& snapshot) {
		ComponentSerializer<T>::deserialize(dst, static_cast<const JSONSnapshot&>(snapshot).data);
	}

	static void serializeTileset(const std::shared_ptr<Tileset>& tileset) { s_serializerAPI->serializeTileset(tileset); }
	static void deserializeTileset(std::shared_ptr<Tileset> tileset) { s_serializerAPI->deserializeTileset(tileset); }

	static void loadScriptManifest(std::shared_ptr<Project> project) { s_serializerAPI->loadScriptManifest(project); }

	static void init();
	static void shutdown();

  private:
	static SerializerAPI* s_serializerAPI;
};

}  // namespace Atlas