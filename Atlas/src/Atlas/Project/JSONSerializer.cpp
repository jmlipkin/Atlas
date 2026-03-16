#include "atpch.h"
#include "JSONSerializer.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/SubTexture.h"

#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Behavior.h"

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

		if (entity.hasComponent<Component::UUID>()) {
			e["UUID"] = (uint64_t)entity.getComponent<Component::UUID>().id;
		}
		if (entity.hasComponent<Component::Tag>()) {
			e["Tag"] = entity.getComponent<Component::Tag>().tag;
		}
		if (entity.hasComponent<Component::Transform>()) {
			Component::Transform& transform = entity.getComponent<Component::Transform>();
			json				  t;
			t["Position"]  = {transform.position.x, transform.position.y, transform.position.z};
			t["Rotation"]  = transform.rotation;
			t["Size"]	   = {transform.size.x, transform.size.y};
			e["Transform"] = t;
		}
		if (entity.hasComponent<Component::Sprite>()) {
			json					 s;
			Component::Sprite&		 sprite = entity.getComponent<Component::Sprite>();
			SubTextureSpecification& specs	= sprite.specs;

			std::string texPath = sprite.texturePath;
			s["Texture"]		= texPath;
			s["Tile Size"] = {specs.tileSize.x, specs.tileSize.y};
			s["Size (in Tiles)"] = {specs.sizeInTiles.x, specs.sizeInTiles.y};
			s["Index"]			 = {specs.index.x, specs.index.y};

			e["Sprite"] = s;
		}
		if (entity.hasComponent<Component::Script>()) {
			e["Script"] = entity.getComponent<Component::Script>().instance->getTypeName();
		}

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

		if (e.contains("Transform")) {
			Component::Transform& transform = entity.getComponent<Component::Transform>();
			transform.position				= {e["Transform"]["Position"][0], e["Transform"]["Position"][1], e["Transform"]["Position"][2]};
			transform.size					= {e["Transform"]["Size"][0], e["Transform"]["Size"][1]};
			transform.rotation				= e["Transform"]["Rotation"];
		}

		if (e.contains("Sprite")) {
			std::string				 texPath = ProjectManager::toAbsolutePath(e["Sprite"]["Texture"]);
			AssetManager::loadTexture(texPath);
			glm::ivec2 index	   = {e["Sprite"]["Index"][0], e["Sprite"]["Index"][1]};
			glm::vec2 tileSize	   = {e["Sprite"]["Tile Size"][0], e["Sprite"]["Tile Size"][1]};
			glm::vec2 sizeInTiles = {e["Sprite"]["Size (in Tiles)"][0], e["Sprite"]["Size (in Tiles)"][1]};

			SubTexture sub(texPath, tileSize, index, sizeInTiles);
			entity.addComponent<Component::Sprite>(texPath, sub.getSpecs());
		}
		if (e.contains("Script")) {
			AT_CORE_WARN("Script '{}' on entity '{}' not deserialized - script serialization not yet implemented",
						 e["Script"].get<std::string>(), e["Name"].get<std::string>());
		}
	}
}

}  // namespace Atlas