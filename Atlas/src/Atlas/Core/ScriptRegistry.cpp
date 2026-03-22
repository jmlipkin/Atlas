#include "atpch.h"
#include "ScriptRegistry.h"

#include "Atlas/ECS/Components/Behavior.h"

namespace Atlas {

void ScriptRegistry::registerScript(const std::string& name, ScriptFactory factory, ScriptPriority priority) {
	if (isRegistered(name)) {
		AT_CORE_WARN("ScriptRegistry already contains {}", name);
		return;
	}
	getRegistry()[(int)priority][name] = std::move(factory);
}

std::unique_ptr<Behavior> ScriptRegistry::create(const std::string& name) {
	for (auto& map : getRegistry()) {
		auto it = map.find(name);
		if (it != map.end()) {
			return it->second();
		}
	}
	AT_CORE_WARN("ScriptRegistry: '{}' not found", name);
	return nullptr;
}

bool ScriptRegistry::isRegistered(const std::string& name) {
	for (const auto& map : getRegistry()) {
		if (map.contains(name)) {
			return true;
		}
	}
	return false;
}

ScriptPriority ScriptRegistry::getPriority(const std::string& name) {
	int priority = 0;
	for (const auto& map : getRegistry()) {
		if (map.contains(name)) {
			return (ScriptPriority)priority;
		}
		priority++;
	}
	AT_CORE_ASSERT(false, "ScriptRegistry does not contain {}", name);
}

int ScriptRegistry::getPriorityIndex(const std::string& name) {
	int priority = 0;
	for (const auto& map : getRegistry()) {
		if (map.contains(name)) {
			return priority;
		}
		priority++;
	}
	AT_CORE_ASSERT(false, "ScriptRegistry does not contain {}", name);
}

std::vector<std::string> ScriptRegistry::getRegisteredNames() {
    std::vector<std::string> names;
	for (const auto& map : getRegistry()) {
		for(const auto& [name, factory] : map) {
            names.push_back(name);
        }
	}
    return names;
}

}  // namespace Atlas