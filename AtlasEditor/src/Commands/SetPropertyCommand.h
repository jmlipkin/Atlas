#pragma once

#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Commands/EditorCommand.h"

namespace Atlas {

template <typename C, typename T>
class SetPropertyCommand : public EditorCommand {
  public:
	SetPropertyCommand(const std::string& name, Entity entity, T C::* member, T oldValue, T newValue) : m_name(name), m_entity(entity), m_member(member), m_oldValue(oldValue), m_newValue(newValue) {}

	void execute() override { m_entity.getComponent<C>().*m_member = m_newValue; }
	void undo() override { m_entity.getComponent<C>().*m_member = m_oldValue; }

	std::string displayName() const override {
		return m_name;
	}

  private:
	std::string m_name;
	Entity		m_entity;

	T C::* m_member;
	T	   m_oldValue;
	T	   m_newValue;
};

}  // namespace Atlas