#include "atpch.h"
#include "ComponentSerializer.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Project/Project.h"

namespace Atlas {

void ComponentSerializer<Component::Transform>::serialize(Entity entity, json& dst) {
	auto& t			= entity.getComponent<Component::Transform>();
	dst["Position"] = {t.position.x, t.position.y, t.position.z};
	dst["Rotation"] = t.rotation;
	dst["Size"]		= {t.size.x, t.size.y};
}

void ComponentSerializer<Component::Transform>::deserialize(Entity& entity, const json& src) {
	auto& t	   = entity.getComponent<Component::Transform>();
	t.position = {src["Position"][0], src["Position"][1], src["Position"][2]};
	t.rotation = src["Rotation"];
	t.size	   = {src["Size"][0], src["Size"][1]};
}

void ComponentSerializer<Component::Sprite>::serialize(Entity entity, json& dst) {
	auto& sprite   = entity.getComponent<Component::Sprite>();
	auto& specs	   = sprite.specs;
	dst["Texture"] = sprite.texturePath;
	dst["Size"]	   = {specs.sizeInTiles.x, specs.sizeInTiles.y};
	dst["Index"]   = {specs.index.x, specs.index.y};
}

void ComponentSerializer<Component::Sprite>::deserialize(Entity& entity, const json& src) {
	std::string texPath		= ProjectManager::toAbsolutePath(src["Texture"]);
	glm::ivec2	index		= {src["Index"][0], src["Index"][1]};
	glm::vec2	sizeInTiles = {src["Size"][0], src["Size"][1]};
	AssetManager::loadTexture(texPath);
	SubTexture sub(texPath,
				   ProjectManager::getActiveProject()->getData().tileSize,
				   index, sizeInTiles);
	if (entity.hasComponent<Component::Sprite>())
		entity.getComponent<Component::Sprite>() = {texPath, sub.getSpecs()};
	else
		entity.addComponent<Component::Sprite>(texPath, sub.getSpecs());
}

void ComponentSerializer<Component::Collider>::serialize(Entity entity, json& dst) {
	auto& c			  = entity.getComponent<Component::Collider>();
	dst["Shape"]	  = (int)c.shape;
	dst["Offset"]	  = {c.offset.x, c.offset.y};
	dst["Size"]		  = (c.shape == ColliderShape::AABB)
							? json{c.size.AABB.x, c.size.AABB.y}
							: json{c.size.radius};
	dst["Trigger"]	  = c.isTrigger;
	dst["Layer Mask"] = c.layerMask;
}

void ComponentSerializer<Component::Collider>::deserialize(Entity& entity, const json& src) {
	auto& c		= entity.hasComponent<Component::Collider>()
					  ? entity.getComponent<Component::Collider>()
					  : entity.addComponent<Component::Collider>();
	c.shape		= src["Shape"];
	c.offset	= {src["Offset"][0], src["Offset"][1]};
	c.isTrigger = src["Trigger"];
	c.layerMask = src["Layer Mask"];
	if (c.shape == ColliderShape::AABB)
		c.size.AABB = {src["Size"][0], src["Size"][1]};
	else
		c.size.radius = src["Size"][0];
}

void ComponentSerializer<Component::RigidBody>::serialize(Entity entity, json& dst) {
	auto& rb			 = entity.getComponent<Component::RigidBody>();
	dst["Velocity"]		 = {rb.velocity.x, rb.velocity.y};
	dst["Static"]		 = rb.isStatic;
	dst["Response Type"] = (int)rb.responseType;
}

void ComponentSerializer<Component::RigidBody>::deserialize(Entity& entity, const json& src) {
	auto& rb		= entity.hasComponent<Component::RigidBody>()
						  ? entity.getComponent<Component::RigidBody>()
						  : entity.addComponent<Component::RigidBody>();
	rb.velocity		= {src["Velocity"][0], src["Velocity"][1]};
	rb.isStatic		= src["Static"];
	rb.responseType = src["Response Type"];
}

void ComponentSerializer<Component::Animations>::serialize(Entity entity, json& dst) {
	auto& anims = entity.getComponent<Component::Animations>();
	if (anims.containsActiveClip())
		dst["Active Clip"] = anims.activeClip;

	json clipsArray = json::array();
	for (const auto& [clipName, clip] : anims.clips) {
		json c;
		c["Name"]		 = clipName;
		c["Texture"]	 = clip.texturePath;
		c["Frame Rate"]	 = clip.frameRate;
		c["Should loop"] = clip.shouldLoop;
		c["Size"]		 = {clip.sizeInTiles.x, clip.sizeInTiles.y};
		json f			 = json::array();
		for (const auto& frame : clip.frames)
			f.push_back({frame.index.x, frame.index.y});
		c["Frames"] = f;
		clipsArray.push_back(c);
	}
	dst["Clips"] = clipsArray;
}

void ComponentSerializer<Component::Animations>::deserialize(Entity& entity, const json& src) {
	auto& anims = entity.hasComponent<Component::Animations>()
					  ? entity.getComponent<Component::Animations>()
					  : entity.addComponent<Component::Animations>();

	if (src.contains("Active Clip"))
		anims.activeClip = src["Active Clip"].get<std::string>();

	for (const auto& c : src["Clips"]) {
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
		anims.clips[clip.name] = clip;
	}
}

void ComponentSerializer<Component::Tilemap>::serialize(Entity entity, json& dst) {
	auto& t			  = entity.getComponent<Component::Tilemap>();
	dst["Tileset"]	  = t.tileset;
	dst["Size"]		  = {t.size.x, t.size.y};
	dst["Layer Mask"] = t.layerMask;
	dst["Grid"]		  = t.grid;
}

void ComponentSerializer<Component::Tilemap>::deserialize(Entity& entity, const json& src) {
	auto& t		= entity.hasComponent<Component::Tilemap>()
					  ? entity.getComponent<Component::Tilemap>()
					  : entity.addComponent<Component::Tilemap>();
	t.tileset	= src["Tileset"];
	t.size		= {src["Size"][0], src["Size"][1]};
	t.layerMask = src["Layer Mask"];
	t.grid		= src["Grid"].get<std::vector<int>>();
	ProjectManager::loadTileset(ProjectManager::tilesetPath(t.tileset));
}

void ComponentSerializer<Component::Script>::serialize(Entity entity, json& dst) {
	auto& script = entity.getComponent<Component::Script>();
	if (!script.instance) return;

	Behavior& behavior = *script.instance;
	dst["Name"]		   = script.name;
	dst["Priority"]	   = (int)script.priority;

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
			case BehaviorPropertyType::CHAR:
				prop["Value"] = *static_cast<char*>(property.valuePtr);
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
			default:
				AT_CORE_WARN("Unknown property type for '{}'", name);
				break;
		}
		props.push_back(prop);
	}
	dst["Properties"] = props;
}

void ComponentSerializer<Component::Script>::deserialize(Entity& entity, const json& src) {
	std::string scriptName = src["Name"];
	auto		instance   = ScriptRegistry::create(scriptName);
	if (!instance) {
		AT_CORE_WARN("Script '{}' not found in registry — skipping", scriptName);
		return;
	}

	auto& script	= entity.hasComponent<Component::Script>()
						  ? entity.getComponent<Component::Script>()
						  : entity.addComponent<Component::Script>();
	script.name		= scriptName;
	script.instance = std::move(instance);
	script.priority = ScriptRegistry::getPriority(scriptName);
	script.instance->setEntity(entity);
	script.instance->onCreate();
	script.instance->exposeProperties();

	if (!src.contains("Properties")) return;
	auto& properties = script.instance->getProperties();
	for (auto& prop : src["Properties"]) {
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
			case BehaviorPropertyType::CHAR:
				*static_cast<char*>(property.valuePtr) = prop["Value"].get<std::string>()[0];
				break;
			case BehaviorPropertyType::VEC2:
				*static_cast<glm::vec2*>(property.valuePtr) = {
					prop["Value"][0].get<float>(), prop["Value"][1].get<float>()};
				break;
			case BehaviorPropertyType::VEC3:
				*static_cast<glm::vec3*>(property.valuePtr) = {
					prop["Value"][0].get<float>(), prop["Value"][1].get<float>(),
					prop["Value"][2].get<float>()};
				break;
			case BehaviorPropertyType::VEC4:
				*static_cast<glm::vec4*>(property.valuePtr) = {
					prop["Value"][0].get<float>(), prop["Value"][1].get<float>(),
					prop["Value"][2].get<float>(), prop["Value"][3].get<float>()};
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

}  // namespace Atlas