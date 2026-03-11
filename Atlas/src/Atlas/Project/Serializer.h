#pragma once

#include "Atlas/Scene/Scene.h"

#include <glm/glm.hpp>

namespace Atlas {

class Serializer {
  public:
	enum class SerializerMode {
		READ  = 0,
		WRITE = 1
	};
	enum class SerializerFormat {
		NONE = 0,
		JSON = 1
	};

  public:
	virtual ~Serializer() = default;

	virtual void serializeScene(const std::shared_ptr<Scene>& scene) = 0;
	virtual void deserializeScene(std::shared_ptr<Scene> scene)		 = 0;

	static std::shared_ptr<Serializer> create(const std::string& filepath, SerializerMode mode, SerializerFormat format = SerializerFormat::JSON);
};

}  // namespace Atlas