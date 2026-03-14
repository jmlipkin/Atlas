#include "atpch.h"
#include "Scene.h"

#include "Atlas/Core/Time.h"
#include "Atlas/Renderer/Renderer.h"

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Systems/Systems.h"

namespace Atlas {

Scene::Scene(const std::string& name) : m_name(name), m_filepath(name + ".atscene") {
	m_registry.on_construct<Component::Sprite>().connect<&System::SpriteInitializer::OnSpriteAttach>();
}

Entity Scene::createEntity(const std::string& name) {
	Entity entity = {m_registry.create(), this};
	entity.addComponent<Component::UUID>();
	entity.addComponent<Component::Tag>(name);
	entity.addComponent<Component::Transform>();
	return entity;
}

void Scene::onUpdate(DeltaTime dt) {
	onPreRender(dt);

	{
		auto view = m_registry.view<Component::Script>();
		for (auto entity : view) {
			Component::Script& script = view.get<Component::Script>(entity);
			script.instance->onUpdate(dt);
		}
	}

	System::Animation::updateFrames(m_registry, dt);
	{
		auto view = m_registry.view<Component::Transform, Component::Sprite>();
		for (auto entity : view) {
			Component::Transform transform = view.get<Component::Transform>(entity);

			Component::Sprite sprite = view.get<Component::Sprite>(entity);

			Renderer::drawQuad(transform.position, transform.size, sprite.subtexture);
		}
	}

	onRender(dt);
	onPostRender(dt);
}

void Scene::dispatchEvent(Event& event) {
	onEvent(event);
}

}  // namespace Atlas