#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Core/ScriptPriority.h"
#include "Atlas/ECS/Entities/Entity.h"

#include <string>
#include <unordered_map>
#include <functional>

namespace Atlas {

class Behavior;

namespace Component {
struct Script;
}

class ATLAS_API ScriptRegistry {
  public:
	using ScriptFactory = std::function<std::unique_ptr<Behavior>()>;

	static void registerScript(const std::string& name, ScriptFactory factory, ScriptPriority priority = ScriptPriority::Normal);
	static bool reinstantiateScript(Component::Script& script, Entity entity);

	static void clear();

	static std::unique_ptr<Behavior> create(const std::string& name);

	static bool			  isRegistered(const std::string& name);
	static ScriptPriority getPriority(const std::string& name);
	static int			  getPriorityIndex(const std::string& name);

	static std::vector<std::string> getRegisteredNames();

  private:
	using Registry = std::array<std::unordered_map<std::string, ScriptFactory>, 3>;
	static Registry& getRegistry() {
		static Registry s_registry;	 // constructed on first use
		return s_registry;
	}
};

}  // namespace Atlas