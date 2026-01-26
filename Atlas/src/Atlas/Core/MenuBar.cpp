#include "Atlas/Core/MenuBar.h"
#include "atpch.h"

#include "Atlas/Platform/MacOS/MacOSMenuBar.h"

namespace Atlas {
std::shared_ptr<MenuBar> MenuBar::create() {
#ifdef AT_PLATFORM_MACOS
    return std::make_shared<MacOSMenuBar>();
#else
	AT_ASSERT("Platforms other than MacOS are not supported!");
	return nullptr;
#endif
}

}  // namespace Atlas