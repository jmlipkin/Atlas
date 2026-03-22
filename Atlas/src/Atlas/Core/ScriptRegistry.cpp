#include "atpch.h"
#include "ScriptRegistry.h"

namespace Atlas {

std::array<std::unordered_map<std::string, ScriptRegistry::ScriptFactory>, 3> ScriptRegistry::s_registry;

void ScriptRegistry::registerScript(const std::string& name, ScriptFactory factory, ScriptPriority priority) {
	if (isRegistered(name)) {
		AT_CORE_WARN("ScriptRegistry already contains {}", name);
		return;
	}
	s_registry[(int)priority][name] = std::move(factory);
}

std::unique_ptr<Behavior> ScriptRegistry::create(const std::string& name) {
	for (auto& map : s_registry) {
		auto it = map.find(name);
		if (it != map.end()) {
			return it->second();
		}
	}
	AT_CORE_WARN("ScriptRegistry: '{}' not found", name);
	return nullptr;
}

bool ScriptRegistry::isRegistered(const std::string& name) {
	for (const auto& map : s_registry) {
		if (map.contains(name)) {
			return true;
		}
	}
	return false;
}

ScriptPriority ScriptRegistry::getPriority(const std::string& name) {
	int priority = 0;
	for (const auto& map : s_registry) {
		if (map.contains(name)) {
			return (ScriptPriority)priority;
		}
		priority++;
	}
	AT_CORE_ASSERT(false, "ScriptRegistry does not contain {}", name);
}

int ScriptRegistry::getPriorityIndex(const std::string& name) {
	int priority = 0;
	for (const auto& map : s_registry) {
		if (map.contains(name)) {
			return priority;
		}
		priority++;
	}
	AT_CORE_ASSERT(false, "ScriptRegistry does not contain {}", name);
}

}  // namespace Atlas