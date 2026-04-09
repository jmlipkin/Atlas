#include "atpch.h"
#include "CommandHistory.h"

#include "Commands/EditorCommand.h"

namespace Atlas {

void CommandHistory::push(std::unique_ptr<EditorCommand> command) {
	command->execute();

	m_redoStack.clear();

	m_undoStack.push_front(std::move(command));
	clampStacktoCapacity(m_undoStack);
}

void CommandHistory::clear() {
	m_undoStack.clear();
	m_redoStack.clear();
}

void CommandHistory::undo() {
	if (!canUndo()) return;

	m_undoStack.front()->undo();

	m_redoStack.push_front(std::move(m_undoStack.front()));
	clampStacktoCapacity(m_redoStack);

	m_undoStack.pop_front();
}

void CommandHistory::redo() {
	if (!canRedo()) return;

	m_redoStack.front()->execute();

	m_undoStack.push_front(std::move(m_redoStack.front()));
	clampStacktoCapacity(m_undoStack);

	m_redoStack.pop_front();
}

std::string CommandHistory::getUndoName() {
	return (canUndo())
			   ? m_undoStack.front()->displayName()
			   : "";
}

std::string CommandHistory::getRedoName() {
	return (canRedo())
			   ? m_redoStack.front()->displayName()
			   : "";
}

void CommandHistory::clampStacktoCapacity(std::deque<std::unique_ptr<EditorCommand>>& stack) {
	if (stack.size() > m_capacity)
		stack.pop_back();
}

}  // namespace Atlas