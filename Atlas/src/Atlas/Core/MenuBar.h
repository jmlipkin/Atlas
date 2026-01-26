#pragma once

#include <memory>

namespace Atlas {

class MenuBar {
  public:
	virtual ~MenuBar() = default;

	virtual void generateMenuBar(const std::string& title) = 0;
	static std::shared_ptr<MenuBar> create();
};

}  // namespace Atlas