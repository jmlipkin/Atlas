#include "atpch.h"
#include "JSONSerializer.h"

#include <json/include/nlohmann/json.hpp>

namespace Atlas {

JSONSerializer::JSONSerializer(const std::string& filepath, SerializerMode mode) {
}

void JSONSerializer::writeString(const std::string& key, const std::string& value) {
}

void JSONSerializer::writeFloat(const std::string& key, float value) {
}

void JSONSerializer::writeInt(const std::string& key, int value) {
}

void JSONSerializer::writeBool(const std::string& key, bool value) {
}

void JSONSerializer::writeVec2(const std::string& key, const glm::vec2& value) {
}

void JSONSerializer::writeVec3(const std::string& key, const glm::vec3& value) {
}

void JSONSerializer::writeVec4(const std::string& key, const glm::vec4& value) {
}

void JSONSerializer::writeMat4(const std::string& key, const glm::mat4& value) {
}

void JSONSerializer::beginWriteObject(const std::string& name) {
}

void JSONSerializer::endWriteObject() {
}

void JSONSerializer::beginWriteArray(const std::string& name) {
}

void JSONSerializer::endWriteArray() {
}

std::string JSONSerializer::readString(const std::string& key) {
	return "Unknown";
}

float JSONSerializer::readFloat(const std::string& key) {
	return 0.0;
}

int JSONSerializer::readInt(const std::string& key) {
	return 0;
}

bool JSONSerializer::readBool(const std::string& key) {
	return false;
}

glm::vec2 readVec2(const std::string& key) {
	return glm::vec2(0.0);
}

glm::vec3 readVec3(const std::string& key) {
	return glm::vec3(0.0);
}

glm::vec4 readVec4(const std::string& key) {
	return glm::vec4(0.0);
}

glm::mat4 readMat4(const std::string& key) {
	return glm::mat4(0.0);
}

void JSONSerializer::beginReadObject(const std::string& name) {
}

void JSONSerializer::endReadObject() {
}

void JSONSerializer::beginReadArray(const std::string& name) {
}

void JSONSerializer::endReadArray() {
}

}  // namespace Atlas