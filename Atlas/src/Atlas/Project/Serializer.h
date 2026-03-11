#pragma once

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
	virtual ~Serializer() = 0;

	virtual void writeString(const std::string& key, const std::string& value) = 0;
	virtual void writeFloat(const std::string& key, float value)			   = 0;
	virtual void writeInt(const std::string& key, int value)				   = 0;
	virtual void writeBool(const std::string& key, bool value)				   = 0;

	virtual void writeVec2(const std::string& key, const glm::vec2& value) = 0;
	virtual void writeVec3(const std::string& key, const glm::vec3& value) = 0;
	virtual void writeVec4(const std::string& key, const glm::vec4& value) = 0;
	virtual void writeMat4(const std::string& key, const glm::mat4& value) = 0;

	virtual void beginWriteObject(const std::string& name) = 0;
	virtual void endWriteObject()						   = 0;
	virtual void beginWriteArray(const std::string& name)  = 0;
	virtual void endWriteArray()						   = 0;

	virtual std::string readString(const std::string& key) = 0;
	virtual float		readFloat(const std::string& key)  = 0;
	virtual int			readInt(const std::string& key)	   = 0;
	virtual bool		readBool(const std::string& key)   = 0;

	virtual glm::vec2 readVec2(const std::string& key) = 0;
	virtual glm::vec3 readVec3(const std::string& key) = 0;
	virtual glm::vec4 readVec4(const std::string& key) = 0;
	virtual glm::mat4 readMat4(const std::string& key) = 0;

	virtual void beginReadObject(const std::string& name) = 0;
	virtual void endReadObject()						  = 0;
	virtual void beginReadArray(const std::string& name)  = 0;
	virtual void endReadArray()							  = 0;

	static std::shared_ptr<Serializer> create(const std::string& filepath, SerializerMode mode, SerializerFormat format = SerializerFormat::JSON);
};

}  // namespace Atlas