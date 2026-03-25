#include "atpch.h"
#include "Scene.h"

#include "Atlas/Core/Time.h"
#include "Atlas/Renderer/Renderer.h"

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Systems/Systems.h"
#include "Atlas/ECS/Systems/Collision.h"

namespace Atlas {

Scene::Scene(const std::string& name) : m_name(name), m_filepath(name + ".atscene") {
	m_registry.on_construct<Component::Sprite>().connect<&System::SpriteInitializer::OnSpriteAttach>();
}

Entity Scene::createEntity(const std::string& name, UUID id) {
	Entity entity = {m_registry.create(), this, id};
	entity.addComponent<Component::UUID>(id);
	entity.addComponent<Component::Tag>(name);
	entity.addComponent<Component::Transform>();
	return entity;
}

void Scene::onUpdate(DeltaTime dt) {
	onPreRender(dt);

	{
		auto view = m_registry.view<Component::Script>();
		for (auto entity : view) {
			if (!m_registry.valid(entity)) continue;
			auto& script = m_registry.get<Component::Script>(entity);
			if (!script.instance) continue;	 // guard against null instance
			script.instance->onUpdate(dt);
		}
	}

	System::Collision::update(dt, *this);

	System::Animation::updateFrames(m_registry, dt);
	{
		auto view = m_registry.view<Component::Transform, Component::Sprite>();
		for (auto entity : view) {
			if (m_registry.all_of<Component::Animations>(entity)) continue;
			Component::Transform transform = view.get<Component::Transform>(entity);

			Component::Sprite sprite = view.get<Component::Sprite>(entity);
			if (sprite.texturePath.empty())
				continue;

			Renderer::drawSprite(transform, sprite);
		}
	}
	{
		auto view = m_registry.view<Component::Transform, Component::Sprite, Component::Animations>();
		for (auto entity : view) {
			Component::Transform   transform = view.get<Component::Transform>(entity);
			Component::Animations& anim		 = view.get<Component::Animations>(entity);

			if (!anim.containsActiveClip()) {
				Component::Sprite sprite = view.get<Component::Sprite>(entity);
				if (!sprite.texturePath.empty())
					Renderer::drawSprite(transform, sprite);
				continue;
			}

			AnimationClip& clip = anim.clips[anim.activeClip];
			if (clip.playing && !clip.frames.empty()) {
				SubTexture		  frame(clip.texturePath, m_tileSize, clip.frames[clip.currentFrame].index, clip.sizeInTiles);
				Component::Sprite sprite(clip.texturePath, frame.getSpecs());
				if (sprite.texturePath.empty())
					continue;

				Renderer::drawSprite(transform, sprite);
				continue;
			}

			Component::Sprite sprite = view.get<Component::Sprite>(entity);
			if (sprite.texturePath.empty())
				continue;
			Renderer::drawSprite(transform, sprite);
		}
	}

	onRender(dt);
	onPostRender(dt);
}

void Scene::onEvent(Event& event) {
	onEventCustom(event);

	auto view = m_registry.view<Component::Script>();
	for (auto entity : view) {
		auto& script = view.get<Component::Script>(entity);
		if (script.instance)
			script.instance->onEvent(event);
	}
}

void Scene::onTileSizeChanged() {
	auto spriteView = m_registry.view<Component::Sprite>();
	for (auto entity : spriteView) {
		auto& sprite = m_registry.get<Component::Sprite>(entity);
		if (sprite.texturePath.empty()) continue;
		sprite.recalculateCoordinates(m_tileSize);
	}
}

}  // namespace Atlas