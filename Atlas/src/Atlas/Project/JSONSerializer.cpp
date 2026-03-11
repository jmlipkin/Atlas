#include "Atlas/Renderer/TextureSheet.h"
#include "atpch.h"
#include "JSONSerializer.h"

#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Behavior.h"

#include <json/include/nlohmann/json.hpp>

namespace Atlas {

using json = nlohmann::ordered_json;

JSONSerializer::JSONSerializer(const std::string& filepath, SerializerMode mode) : m_filepath(filepath) {
}

void JSONSerializer::serializeScene(const std::shared_ptr<Scene>& scene) {
    std::ofstream file(m_filepath);
    AT_CORE_ASSERT(file.is_open(), "Could not open file {} for writing!", m_filepath);

    json root;
    root["name"] = "Scene name here";

    json entities = json::array();
    auto& reg = scene->getRegistry();
    auto view = scene->getRegistry().view<Component::Tag>();

    for (entt::entity entt : view) {
        Entity entity = {entt, scene.get()};
        json e;
        if(entity.hasComponent<Component::Tag>()) {
            Component::Tag& tag = entity.getComponent<Component::Tag>();
            e["Tag"] = tag.tag;
        }
        if(entity.hasComponent<Component::Transform>()) {
            Component::Transform& transform = entity.getComponent<Component::Transform>();
            json t;
            t["position"] = {transform.position.x, transform.position.y, transform.position.z};
            t["rotation"] = transform.rotation;
            t["size"] = {transform.size.x, transform.size.y};
            e["Transform"] = t;
        }
        if(entity.hasComponent<Component::Sprite>()) {
            json s;
            Component::Sprite& sprite = entity.getComponent<Component::Sprite>();
            SubTextureSpecification& specs = sprite.subtexture->getSpecs();

            s["Texture"] = sprite.subtexture->getTexture()->getFilepath();
            json coords;

            coords["Top left"] = {specs.coordinates.top_left.x, specs.coordinates.top_left.y};
            coords["Top right"] = {specs.coordinates.top_right.x, specs.coordinates.top_right.y};
            coords["Bottom left"] = {specs.coordinates.bottom_left.x, specs.coordinates.bottom_left.y};
            coords["Bottom right"] = {specs.coordinates.bottom_right.x, specs.coordinates.bottom_right.y};

            s["Coordinates"] = coords;
            s["Tile Dimensions"] = {specs.tileDims.x, specs.tileDims.y};
            s["Index"] = {specs.index.x, specs.index.y};

            e["Sprite"] = s;
        }
        if(entity.hasComponent<Component::Script>()) {
            e["Script"] = "TODO!!";
        }

        entities.push_back(e);
    }
    root["entities"] = entities;
    file << root.dump(2);
    file.close();

}

void JSONSerializer::deserializeScene(std::shared_ptr<Scene> scene) {
}

}  // namespace Atlas