#pragma once

#include "Atlas/Project/Serializer.h"
#include "Commands/EditorCommand.h"

#include "Atlas/Project/Snapshot.h"
#include "Atlas/ECS/Entities/Entity.h"

namespace Atlas {

template <typename T>
class AddComponentCommand : public EditorCommand {
  public:
	AddComponentCommand(Entity entity) : m_entity(entity) {}

	virtual void execute() override { m_entity.addComponent<T>(); }
	virtual void undo() override { m_entity.removeComponent<T>(); }

	std::string displayName() const override {
		return std::string("Add ") + ComponentSerializer<T>::key();
	}

  private:
	Entity m_entity;
};

template <typename T>
class RemoveComponentCommand : public EditorCommand {
  public:
	RemoveComponentCommand(Entity entity) : m_entity(entity), m_snapshot(Serializer::saveComponentSnapshot<T>(entity)) {}

	virtual void execute() override { m_entity.removeComponent<T>(); }
	virtual void undo() override {
		m_entity.addComponent<T>();
		Serializer::recallComponentSnapshot<T>(m_entity, *m_snapshot);
	}

    std::string displayName() const override { 
        return std::string("Remove ") + ComponentSerializer<T>::key();
    }

  private:
	Entity					  m_entity;
	std::unique_ptr<Snapshot> m_snapshot;
};

}  // namespace Atlas