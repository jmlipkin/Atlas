#pragma once

#include "Atlas/Project/Snapshot.h"

#include <json/include/nlohmann/json.hpp>

namespace Atlas {

using json = nlohmann::ordered_json;

class JSONSnapshot : public Snapshot {
  public:
	json data;
};

}  // namespace Atlas