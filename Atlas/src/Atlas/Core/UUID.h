#pragma once

#include <random>
#include <string>

namespace Atlas {
class UUID {
  public:
	UUID() : m_id(generate()) {}
	UUID(uint64_t id) : m_id(id) {}

	operator uint64_t() const { return m_id; }
	bool operator==(UUID other) const { return m_id == other.m_id; }
	operator bool() const { return m_id != 0; }

	bool		isNull() const { return m_id == 0; }
	std::string toString() const { return std::to_string(m_id); }

	static UUID null() { return UUID(0); }

  private:
	static UUID generate() {
		static std::random_device					   rd;
		static std::mt19937_64						   gen(rd());
		static std::uniform_int_distribution<uint64_t> dis(1);
		return UUID(dis(gen));
	}

  private:
	uint64_t m_id;
};
}  // namespace Atlas

template <>
struct std::hash<Atlas::UUID> {
	size_t operator()(const Atlas::UUID& uuid) const {
		return std::hash<uint64_t>()((uint64_t)uuid);
	}
};