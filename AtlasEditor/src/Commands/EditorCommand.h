#pragma once

#include <string>

namespace Atlas {

/**
 * @brief Base class for all editor operations. Stored in CommandHistory for
 * undo/redo operations.
 *
 */
struct EditorCommand {
	virtual ~EditorCommand() = default;

	virtual void		execute()			= 0;
	virtual void		undo()				= 0;
	virtual std::string displayName() const = 0;
};

}  // namespace Atlas