#pragma once

#include "EditorCommand.h"

#include <deque>
#include <memory>

namespace Atlas {

/**
 * @brief Storage unit for undo and redo operations.
 *
 */
class CommandHistory {
  public:
	/**
	 * @brief Executes command, adds command to undo history, and clears the
	 * redo stack
	 *
	 * @param command - Command to execute
	 */
	void push(std::unique_ptr<EditorCommand> command);

	/**
	 * @brief Resets undo and redo histories
	 *
	 */
	void clear();

	/**
	 * @brief Undoes most recent operation and adds it to the redo stack
	 *
	 */
	void undo();

	/**
	 * @brief Executes most-recently undone operation and adds it back to the
	 * undo stack
	 *
	 */
	void redo();

	std::string getUndoName();
	std::string getRedoName();

	bool canUndo() { return !m_undoStack.empty(); }
	bool canRedo() { return !m_redoStack.empty(); }

  private:
	void clampStacktoCapacity(std::deque<std::unique_ptr<EditorCommand>>& stack);

  private:
	size_t m_capacity = 100;

	std::deque<std::unique_ptr<EditorCommand>> m_undoStack;
	std::deque<std::unique_ptr<EditorCommand>> m_redoStack;
};

}  // namespace Atlas