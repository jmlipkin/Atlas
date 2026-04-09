#pragma once

#include "EditorCommand.h"

#include <string>
#include <vector>
#include <memory>

namespace Atlas {

/**
 * @brief Supports compound commands in CommandHistory. Extends EditorCommand.
 *
 */
class EditorCompoundCommand : EditorCommand {
  public:
	EditorCompoundCommand(const std::string& name) : m_name(name) {}

	/**
	 * @brief Adds a Command to the compounded command. Will be called in order
	 * of adding on execute, and reverse order on undo.
	 *
	 * @param command - operation to add to CompoundCommand
	 */
	void append(std::unique_ptr<EditorCommand> command) {
		m_commands.push_back(std::move(command));
	}

	/**
	 * @brief Iterates commands and executes each in appended order.
	 *
	 */
	virtual void execute() final {
		for (auto& command : m_commands) command->execute();
	}

	/**
	 * @brief Iterates commands and undoes each in reverse order.
	 *
	 */
	virtual void undo() final {
		for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it) {
			it->get()->undo();
		}
	}

	virtual std::string displayName() const override { return m_name; }

  private:
	std::vector<std::unique_ptr<EditorCommand>> m_commands;

	std::string m_name;
};

}  // namespace Atlas