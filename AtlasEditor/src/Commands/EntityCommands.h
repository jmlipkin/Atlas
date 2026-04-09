#pragma once

#include "Commands/EditorCommand.h"
#include "Commands/SetPropertyCommand.h"

#include "Atlas/ECS/Components/Components.h"
#include "Atlas/Project/Serializer.h"

namespace Atlas {
class CreateEntityCommand : public EditorCommand {
  public:
	CreateEntityCommand(std::shared_ptr<Scene> scene, const std::string& entityName) : m_scene(scene), m_entityName(entityName) {}

	void execute() override {
		m_entity = m_scene->createEntity(m_entityName);
	}

	void undo() override {
		m_scene->deleteEntity(m_entity);
	}

	std::string displayName() const override { return "Create Entity"; }

  private:
	std::shared_ptr<Scene> m_scene;
	std::string			   m_entityName;
	Entity				   m_entity;
};

class DeleteEntityCommand : public EditorCommand {
  public:
	DeleteEntityCommand(std::shared_ptr<Scene> parentScene, Entity entity) : m_scene(parentScene), m_entity(entity) {}

	void execute() override {
		m_snapshot = Serializer::saveEntitySnapshot(m_entity);
		m_scene->deleteEntity(m_entity);
	}

	void undo() override {
		Entity restored = m_scene->createEntity(m_entity.name(), m_entity.getUUID());
		Serializer::recallEntitySnapshot(restored, *m_snapshot);
	}

	std::string displayName() const override { return "Delete Entity"; }

  private:
	std::shared_ptr<Scene> m_scene;
	Entity				   m_entity;

	std::unique_ptr<EntitySnapshot> m_snapshot;
};

class RenameEntityCommand : public SetPropertyCommand<Component::Tag, std::string> {
  public:
	RenameEntityCommand(Entity entity, const std::string& oldName, const std::string& newName) : SetPropertyCommand<Component::Tag, std::string>("Rename Entity", entity, &Component::Tag::tag, oldName, newName) {}
};

}  // namespace Atlas