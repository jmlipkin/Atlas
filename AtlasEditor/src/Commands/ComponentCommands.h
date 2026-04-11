#pragma once

#include "Atlas/Project/Serializer.h"
#include "Commands/EditorCommand.h"

#include "Atlas/Project/Snapshot.h"
#include "Atlas/ECS/Entities/Entity.h"

namespace Atlas {

template <typename T, typename... Args>
class AddComponentCommand : public EditorCommand {
  public:
	AddComponentCommand(Entity entity, Args&&... args) : m_entity(entity), m_args(std::forward<Args>(args)...) {}

	virtual void execute() override {
		std::apply([&](auto&&... args) {
			m_entity.refresh();
			m_entity.addComponent<T>(std::forward<decltype(args)>(args)...);
		},
				   m_args);
	}
	virtual void undo() override {
		m_entity.refresh();
		m_entity.removeComponent<T>();
	}

	std::string displayName() const override {
		return std::string("Add ") + ComponentSerializer<T>::key();
	}

  private:
	Entity				m_entity;
	std::tuple<Args...> m_args;
};

template <typename T, typename... Args>
auto makeAddComponentCommand(Entity entity, Args&&... args) {
	return std::make_unique<AddComponentCommand<T, Args...>>(entity, std::forward<Args>(args)...);
}

template <typename T>
class RemoveComponentCommand : public EditorCommand {
  public:
	RemoveComponentCommand(Entity entity) : m_entity(entity), m_snapshot(Serializer::saveComponentSnapshot<T>(entity)) {}

	virtual void execute() override {
		m_entity.refresh();
		m_entity.removeComponent<T>();
	}
	virtual void undo() override {
		m_entity.refresh();
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