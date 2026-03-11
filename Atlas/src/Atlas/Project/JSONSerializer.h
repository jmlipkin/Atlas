#pragma once

#include "Atlas/Project/Serializer.h"
#include "Atlas/Scene/Scene.h"

namespace Atlas {

class JSONSerializer : public Serializer {
  public:
	JSONSerializer(const std::string& filepath, SerializerMode mode);
	virtual ~JSONSerializer() = default;

	virtual void serializeScene(const std::shared_ptr<Scene>& scene) override;
	virtual void deserializeScene(std::shared_ptr<Scene> scene) override;

    private:
    const std::string& m_filepath;
};

}  // namespace Atlas