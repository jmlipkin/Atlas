#include "atpch.h"
#include "Atlas/Scene/Scene.h"
#include "Atlas/Scene/Entity.h"
#include "Atlas/Scene/Components.h"

namespace Atlas {

    Entity Scene::createEntity(const std::string& name) {
        Entity entity = {m_registry.create(), this};
        entity.addComponent<Component::Tag>(name);
        return entity;
    }
}