#include "atpch.h"
#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"

namespace Atlas {

    Entity Scene::createEntity(const std::string& name) {
        Entity entity = {m_registry.create(), this};
        entity.addComponent<Component::Tag>(name);
        entity.addComponent<Component::Transform>();
        return entity;
    }
}