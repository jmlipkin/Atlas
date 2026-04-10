#include "atpch.h"
#include "JSONSerializer.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Renderer/Tileset.h"

#include "Atlas/Project/Project.h"
#include "Atlas/Project/ComponentSerializer.h"
#include "Atlas/Project/JSONSnapshot.h"

#include <json/include/nlohmann/json.hpp>

namespace Atlas {

using json = nlohmann::ordered_json;

void JSONSerializer::serializeProject(const std::shared_ptr<Project>& project) {
	std::filesystem::create_directories(project->getDirectory());

	std::string	  filepath = project->getDirectory() + "/" + project->getName() + ".atproj";
	std::ofstream file(filepath);
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for writing!", filepath);

	json root;
	root["Atlas Version"]		 = project->getData().atlas_version;
	root["AtlasProject Version"] = project->getData().atproj_version;

	root["Name"]			  = project->getName();
	root["Startup Scene"]	  = project->getData().startup_scene;
	root["Last Active Scene"] = project->getData().last_active_scene;

	root["Scenes"] = project->getData().scene_filepaths;

	json world;
	world["Tile Size"]		 = project->getData().tileSize;
	world["Pixels Per Unit"] = project->getData().pixelsPerUnit;

	root["World"] = world;

	file << root.dump(2);
	file.close();
}

void JSONSerializer::deserializeProject(std::shared_ptr<Project> project) {
	std::string	  filepath = project->getDirectory() + "/" + project->getName() + ".atproj";
	std::ifstream file(filepath);
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for reading!", filepath);

	json root = json::parse(file);
	file.close();

	if (root["AtlasProject Version"] != ProjectData::atproj_version) {
		AT_CORE_WARN("Project file version mismatch — expected {}, got {}",
					 ProjectData::atproj_version, (int)root["AtlasProject Version"]);
	}

	project->getName()					 = root["Name"];
	project->getData().startup_scene	 = root["Startup Scene"];
	project->getData().last_active_scene = root["Last Active Scene"];

	project->getData().tileSize		 = root["World"]["Tile Size"];
	project->getData().pixelsPerUnit = root["World"]["Pixels Per Unit"];

	project->getData().scene_filepaths = root["Scenes"].get<std::vector<std::string>>();
}

void JSONSerializer::serializeScene(const std::shared_ptr<Scene>& scene) {
	std::filesystem::create_directories(std::filesystem::path(scene->getPath()).parent_path());

	std::ofstream file(scene->getPath());
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for writing!", scene->getPath());

	json root;
	root["Name"] = scene->getName();

	json  entities = json::array();
	auto& reg	   = scene->getRegistry();
	auto  view	   = scene->getRegistry().view<Component::UUID>();

	for (entt::entity entt : view) {
		Entity entity = {entt, scene.get()};
		json   e;
		serializeEntity(entity, e);
		entities.push_back(e);
	}
	root["Entities"] = entities;

	scene->serializeData(root);

	file << root.dump(2);
	file.close();
}

void JSONSerializer::deserializeScene(std::shared_ptr<Scene> scene) {
	std::ifstream file(scene->getPath());
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for reading!", scene->getPath());

	// Protects against reconciling existing scenes
	std::unordered_map<UUID, Entity> existing;
	auto							 view = scene->getRegistry().view<Component::UUID>();
	for (entt::entity e : view) {
		Entity entity{e, scene.get()};
		existing[entity.getComponent<Component::UUID>().id] = entity;
	}

	json root = json::parse(file);
	file.close();

	scene->getName() = root["Name"];
	scene->deserializeData(root);

	for (auto& e : root["Entities"]) {
		UUID uuid((uint64_t)e["UUID"]);

		Entity entity;

		if (existing.contains(uuid)) {
			entity = existing[uuid];
		} else {
			entity = scene->createEntity(e["Tag"], uuid);
		}
		deserializeEntity(entity, e);
	}
}

std::unique_ptr<Snapshot> JSONSerializer::saveEntitySnapshot(Entity entity) {
	std::unique_ptr<JSONSnapshot> snapshot = std::make_unique<JSONSnapshot>();
	serializeEntity(entity, snapshot->data);
	return std::move(snapshot);
}

void JSONSerializer::recallEntitySnapshot(Entity& dst, const Snapshot& snapshot) {
	deserializeEntity(dst, static_cast<const JSONSnapshot&>(snapshot).data);
}

void JSONSerializer::serializeTileset(const std::shared_ptr<Tileset>& tileset) {
	std::filesystem::create_directories(std::filesystem::path(tileset->getPath()).parent_path());

	std::ofstream file(tileset->getPath());
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for writing!", tileset->getPath());

	json ts;

	ts["Name"]	  = tileset->getName();
	auto& tilemap = tileset->getTileset();

	ts["Texture"] = tileset->getTexture();

	if (tilemap.empty()) {
		file << ts.dump(2);
		file.close();
		return;
	}

	json tiles;
	for (auto& [index, tile] : tilemap) {
		json t;
		t["Index"]		= index;
		t["Size"]		= {tile.sizeInTiles.x, tile.sizeInTiles.y};
		t["Grid Index"] = {tile.gridIndex.x, tile.gridIndex.y};
		t["Solid"]		= tile.isSolid;

		tiles.push_back(t);
	}

	ts["Tiles"] = tiles;
	file << ts.dump(2);

	file.close();
}

void JSONSerializer::deserializeTileset(std::shared_ptr<Tileset> tileset) {
	std::ifstream file(tileset->getPath());
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for reading!", tileset->getPath());

	json ts = json::parse(file);
	file.close();

	tileset->setName(ts["Name"]);
	auto& map = tileset->getTileset();

	if (ts.contains("Texture")) {
		std::string texPath = ProjectManager::toAbsolutePath(ts["Texture"]);
		AssetManager::loadTexture(texPath);
		tileset->setTexture(texPath);
	}

	if (!ts.contains("Tiles")) {
		return;
	}

	for (auto& t : ts["Tiles"]) {
		TileDefinition tdef;

		tdef.sizeInTiles = glm::ivec2(t["Size"][0], t["Size"][1]);
		tdef.gridIndex	 = glm::ivec2(t["Grid Index"][0], t["Grid Index"][1]);
		tdef.isSolid	 = t["Solid"];

		map[t["Index"]] = tdef;
	}
}

void JSONSerializer::loadScriptManifest(std::shared_ptr<Project> project) {
	std::string manifestPath = project->getDirectory() + "/scripts.manifest";
	if (!std::filesystem::exists(manifestPath)) {
		AT_CORE_WARN("No scripts.manifest found at {}", manifestPath);
		return;
	}

	std::ifstream file(manifestPath);
	json		  root = json::parse(file);
	file.close();

	if (root.contains("Target")) {
		project->getData().scripts_target = root["Target"].get<std::string>();
	}

	if (root.contains("Scripts")) {
		// Script names are registered by the dylib at load time via AT_REGISTER_SCRIPT.
		// This block is reserved for future manifest-driven registration.
	}
}

void JSONSerializer::serializeEntity(Entity entity, json& dst) {
	if (entity.hasComponent<Component::UUID>()) {
		dst["UUID"] = (uint64_t)entity.getComponent<Component::UUID>().id;
	}
	if (entity.hasComponent<Component::Tag>()) {
		dst["Tag"] = entity.name();
	}
	serializeComponents<ATLAS_ALL_COMPONENTS>(entity, dst);
}

void JSONSerializer::deserializeEntity(Entity& entity, const json& src) {
	deserializeComponents<ATLAS_ALL_COMPONENTS>(entity, src);
}

}  // namespace Atlas