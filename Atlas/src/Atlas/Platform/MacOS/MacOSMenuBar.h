#pragma once

#include "Atlas/Core/MenuBar.h"

namespace Atlas {
class MacOSMenuBar : public MenuBar {
  public:
	~MacOSMenuBar() = default;

	virtual void generateMenuBar(const std::string& title) override;
};

}  // namespace Atlas