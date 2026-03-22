#pragma once

#include "Atlas/ECS/Components/Behavior.h"

#include <string>
#include <unordered_map>

namespace Atlas {

enum ScriptPriority {
	Early  = 0,
	Normal = 1,
	Late   = 2
};

class ScriptRegistry {
  public:
	using ScriptFactory = std::function<std::unique_ptr<Behavior>()>;

	static void registerScript(const std::string& name, ScriptFactory factory, ScriptPriority priority = ScriptPriority::Normal);

	static std::unique_ptr<Behavior> create(const std::string& name);

	static bool			  isRegistered(const std::string& name);
	static ScriptPriority getPriority(const std::string& name);
	static int			  getPriorityIndex(const std::string& name);

  private:
	static std::array<std::unordered_map<std::string, ScriptFactory>, 3>
		s_registry;
};

}  // namespace Atlas