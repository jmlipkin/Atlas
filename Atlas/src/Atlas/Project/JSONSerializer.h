#pragma once

#include "Atlas/Project/Serializer.h"

namespace Atlas {

class JSONSerializer : public Serializer {
  public:
    JSONSerializer(const std::string& filepath, SerializerMode mode);
	virtual ~JSONSerializer() = default;

	virtual void writeString(const std::string& key, const std::string& value);
	virtual void writeFloat(const std::string& key, float value);
	virtual void writeInt(const std::string& key, int value);
	virtual void writeBool(const std::string& key, bool value);

	virtual void writeVec2(const std::string& key, const glm::vec2& value);
	virtual void writeVec3(const std::string& key, const glm::vec3& value);
	virtual void writeVec4(const std::string& key, const glm::vec4& value);
	virtual void writeMat4(const std::string& key, const glm::mat4& value);

	virtual void beginWriteObject(const std::string& name);
	virtual void endWriteObject();
	virtual void beginWriteArray(const std::string& name);
	virtual void endWriteArray();

	virtual std::string readString(const std::string& key);
	virtual float		readFloat(const std::string& key);
	virtual int			readInt(const std::string& key);
	virtual bool		readBool(const std::string& key);

	virtual glm::vec2 readVec2(const std::string& key);
	virtual glm::vec3 readVec3(const std::string& key);
	virtual glm::vec4 readVec4(const std::string& key);
	virtual glm::mat4 readMat4(const std::string& key);

	virtual void beginReadObject(const std::string& name);
	virtual void endReadObject();
	virtual void beginReadArray(const std::string& name);
	virtual void endReadArray();
};

}  // namespace Atlas