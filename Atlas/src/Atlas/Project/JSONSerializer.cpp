#include "atpch.h"
#include "JSONSerializer.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/SubTexture.h"

#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Components/Collider.h"

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

			std::string texPath	 = sprite.texturePath;
			s["Texture"]		 = texPath;
			s["Size"] = {specs.sizeInTiles.x, specs.sizeInTiles.y};
			s["Index"]			 = {specs.index.x, specs.index.y};

			e["Sprite"] = s;
		}
		if (entity.hasComponent<Component::Script>()) {
			if (entity.getComponent<Component::Script>().instance != nullptr) {
				json			   s;
				Component::Script& script	= entity.getComponent<Component::Script>();
				Behavior&		   behavior = *script.instance.get();
				s["Name"]					= script.name;
				s["Priority"]				= (int)script.priority;
				json props;
				for (auto& [name, property] : behavior.getProperties()) {
					json prop;
					prop["Name"] = name;
					prop["Type"] = (int)property.type;

					switch (property.type) {
						case BehaviorPropertyType::BOOL:
							prop["Value"] = *static_cast<bool*>(property.valuePtr);
							break;
						case BehaviorPropertyType::INT:
							prop["Value"] = *static_cast<int*>(property.valuePtr);
							break;
						case BehaviorPropertyType::FLOAT:
							prop["Value"] = *static_cast<float*>(property.valuePtr);
							break;
						case BehaviorPropertyType::STRING:
							prop["Value"] = *static_cast<std::string*>(property.valuePtr);
							break;
						case BehaviorPropertyType::VEC2: {
							auto& v		  = *static_cast<glm::vec2*>(property.valuePtr);
							prop["Value"] = {v.x, v.y};
							break;
						}
						case BehaviorPropertyType::VEC3: {
							auto& v		  = *static_cast<glm::vec3*>(property.valuePtr);
							prop["Value"] = {v.x, v.y, v.z};
							break;
						}
						case BehaviorPropertyType::VEC4: {
							auto& v		  = *static_cast<glm::vec4*>(property.valuePtr);
							prop["Value"] = {v.x, v.y, v.z, v.w};
							break;
						}
						case BehaviorPropertyType::MAT4: {
							auto& m	  = *static_cast<glm::mat4*>(property.valuePtr);
							json  mat = json::array();
							for (int i = 0; i < 4; i++)
								for (int j = 0; j < 4; j++)
									mat.push_back(m[i][j]);
							prop["Value"] = mat;
							break;
						}
						case BehaviorPropertyType::CHAR:
							prop["Value"] = *static_cast<char*>(property.valuePtr);
							break;
						default:
							AT_CORE_WARN("Unknown property type for '{}'", name);
							break;
					}
					props.push_back(prop);
				}
				s["Properties"] = props;
				e["Script"]		= s;
			}
		}
		if (entity.hasComponent<Component::Collider>()) {
			json c;

			Component::Collider& collider = entity.getComponent<Component::Collider>();
			c["Shape"]					  = (int)collider.shape;
			c["Offset"]					  = {collider.offset.x, collider.offset.y};

			if (collider.shape == ColliderShape::AABB) {
				c["Size"] = {collider.size.AABB.x, collider.size.AABB.y};
			} else {
				c["Size"] = {collider.size.radius};
			}
			c["Trigger"]	= collider.isTrigger;
			c["Layer Mask"] = collider.layerMask;

			e["Collider"] = c;
		}
		if (entity.hasComponent<Component::RigidBody>()) {
			json				  rb;
			Component::RigidBody& body = entity.getComponent<Component::RigidBody>();
			rb["Velocity"]			   = {body.velocity.x, body.velocity.y};
			rb["Static"]			   = body.isStatic;
			rb["Response Type"]		   = (int)body.responseType;

			e["RigidBody"] = rb;
		}
		if (entity.hasComponent<Component::Animations>()) {
			json a;

			Component::Animations& animations = entity.getComponent<Component::Animations>();

			if (animations.containsActiveClip()) {
				a["Active Clip"] = entity.getComponent<Component::Animations>().activeClip;
			}
			json clipsArray = json::array();
			for (const auto& [clipName, clip] : animations.clips) {
				json c;
				c["Name"]			 = clipName;
				c["Texture"]		 = clip.texturePath;
				c["Frame Rate"]		 = clip.frameRate;
				c["Should loop"]	 = clip.shouldLoop;
				c["Size"] = {clip.sizeInTiles.x, clip.sizeInTiles.y};

				json f = json::array();
				for (const auto& frame : clip.frames) {
					f.push_back({frame.index.x, frame.index.y});
				}

				c["Frames"] = f;
				clipsArray.push_back(c);
			}
			a["Clips"]		= clipsArray;
			e["Animations"] = a;
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
			std::string texPath = ProjectManager::toAbsolutePath(e["Sprite"]["Texture"]);
			AssetManager::loadTexture(texPath);
			glm::ivec2 index	   = {e["Sprite"]["Index"][0], e["Sprite"]["Index"][1]};
			glm::vec2  sizeInTiles = {e["Sprite"]["Size"][0], e["Sprite"]["Size"][1]};

			SubTexture sub(texPath, ProjectManager::getActiveProject()->getData().tileSize, index, sizeInTiles);
			entity.addComponent<Component::Sprite>(texPath, sub.getSpecs());
		}
		if (e.contains("Script")) {
			std::string scriptName = e["Script"]["Name"];
			auto		instance   = ScriptRegistry::create(scriptName);
			if (instance) {
				auto& script	= entity.addComponent<Component::Script>();
				script.name		= e["Script"]["Name"];
				script.instance = std::move(instance);
				script.priority = ScriptRegistry::getPriority(scriptName);
				script.instance->setEntity(entity);
				script.instance->onCreate();
				script.instance->exposeProperties();

				if (e.contains("Properties")) {
					auto& properties = script.instance->getProperties();
					for (auto& prop : e["Properties"]) {
						std::string name = prop["Name"].get<std::string>();
						if (!properties.contains(name)) continue;

						auto&				 property = properties[name];
						BehaviorPropertyType type	  = (BehaviorPropertyType)prop["Type"].get<int>();

						switch (type) {
							case BehaviorPropertyType::BOOL:
								*static_cast<bool*>(property.valuePtr) = prop["Value"].get<bool>();
								break;
							case BehaviorPropertyType::INT:
								*static_cast<int*>(property.valuePtr) = prop["Value"].get<int>();
								break;
							case BehaviorPropertyType::FLOAT:
								*static_cast<float*>(property.valuePtr) = prop["Value"].get<float>();
								break;
							case BehaviorPropertyType::STRING:
								*static_cast<std::string*>(property.valuePtr) = prop["Value"].get<std::string>();
								break;
							case BehaviorPropertyType::VEC2:
								*static_cast<glm::vec2*>(property.valuePtr) = {
									prop["Value"][0].get<float>(),
									prop["Value"][1].get<float>()};
								break;
							case BehaviorPropertyType::VEC3:
								*static_cast<glm::vec3*>(property.valuePtr) = {
									prop["Value"][0].get<float>(),
									prop["Value"][1].get<float>(),
									prop["Value"][2].get<float>()};
								break;
							case BehaviorPropertyType::VEC4:
								*static_cast<glm::vec4*>(property.valuePtr) = {
									prop["Value"][0].get<float>(),
									prop["Value"][1].get<float>(),
									prop["Value"][2].get<float>(),
									prop["Value"][3].get<float>()};
								break;
							case BehaviorPropertyType::CHAR:
								*static_cast<char*>(property.valuePtr) = prop["Value"].get<std::string>()[0];
								break;
							case BehaviorPropertyType::MAT4: {
								auto& m = *static_cast<glm::mat4*>(property.valuePtr);
								int	  k = 0;
								for (int i = 0; i < 4; i++)
									for (int j = 0; j < 4; j++)
										m[i][j] = prop["Value"][k++].get<float>();
								break;
							}
							default:
								AT_CORE_WARN("Unknown property type for '{}'", name);
								break;
						}
					}
				}
			} else {
				AT_CORE_WARN("Script '{}' on entity '{}' not found in registry — skipping",
							 scriptName, e["Tag"].get<std::string>());
			}
		}
		if (e.contains("Collider")) {
			Component::Collider& collider = entity.addComponent<Component::Collider>();

			json c = e["Collider"];

			collider.shape	  = c["Shape"];
			collider.offset.x = c["Offset"][0];
			collider.offset.y = c["Offset"][1];

			if (collider.shape == ColliderShape::AABB) {
				collider.size.AABB.x = c["Size"][0];
				collider.size.AABB.y = c["Size"][1];
			} else {
				collider.size.radius = c["Size"][0];
			}
			collider.isTrigger = c["Trigger"];
			collider.layerMask = c["Layer Mask"];
		}
		if (e.contains("RigidBody")) {
			Component::RigidBody& body = entity.addComponent<Component::RigidBody>();

			json rb = e["RigidBody"];

			body.velocity.x	  = rb["Velocity"][0];
			body.velocity.y	  = rb["Velocity"][1];
			body.isStatic	  = rb["Static"];
			body.responseType = rb["Response Type"];
		}
		if (e.contains("Animations")) {
			Component::Animations& animations = entity.addComponent<Component::Animations>();

			json a = e["Animations"];

			if (a.contains("Active Clip"))
				animations.activeClip = a["Active Clip"].get<std::string>();

			for (const auto& c : a["Clips"]) {
				AnimationClip clip;
				clip.name		 = c["Name"];
				clip.texturePath = ProjectManager::toAbsolutePath(c["Texture"]);
				clip.frameRate	 = c["Frame Rate"];
				clip.shouldLoop	 = c["Should loop"];
				clip.sizeInTiles = {c["Size"][0], c["Size"][1]};

				for (const auto& f : c["Frames"]) {
					AnimationFrame frame;
					frame.index = {f[0], f[1]};
					clip.frames.push_back(frame);
				}

				animations.clips[clip.name] = clip;
			}
		}
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
		for (auto& name : root["Scripts"]) {
		}
	}
}

}  // namespace Atlas