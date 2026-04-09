#pragma once

#include "Commands/EditorCommand.h"

#include <functional>

namespace Atlas {

class LambdaCommand : public EditorCommand {
  public:
	LambdaCommand(const std::string& name, std::function<void()> apply, std::function<void()> revert) : m_name(name), m_apply(apply), m_revert(revert) {}

	void execute() override { m_apply(); }
	void undo() override { m_revert(); }

	std::string displayName() const override { return m_name; }

  private:
	std::string			  m_name;
	std::function<void()> m_apply;
	std::function<void()> m_revert;
};

}  // namespace Atlas