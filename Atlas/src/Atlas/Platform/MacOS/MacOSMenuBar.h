#pragma once

#include "Atlas/Core/MenuBar.h"

#ifdef __OBJC__
@class NSMenuItem;
#else
typedef void NSMenuItem;
#endif

namespace Atlas {
class MacOSMenuBar : public MenuBar {
  public:
	~MacOSMenuBar() = default;

	virtual void generateMenuBar(const std::string& title) override;
	virtual void updateUndoRedoMenuItems(const std::string& undoName, const std::string& redoName) override;

  private:
	NSMenuItem* m_undoMenuItem;
	NSMenuItem* m_redoMenuItem;
};

}  // namespace Atlas