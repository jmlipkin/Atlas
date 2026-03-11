#pragma once

#include "Atlas/Renderer/Shader.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/TextureSheet.h"
#include "Atlas/Core/Font.h"

namespace Atlas {

class AssetManager {
  public:
	static std::shared_ptr<Font> loadFont(const std::string& name, const std::string& filepath, uint32_t fontSizePx = 128);
	static std::shared_ptr<Shader> loadShader(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction);
	static std::shared_ptr<Texture> loadTexture(const std::string& filepath);
	static std::shared_ptr<TextureSheet> loadTextureSheet(const std::string& filepath, const TextureSheetSpecification& specs);

	template <typename T>
	static std::shared_ptr<T> get(const std::string& key) {
		auto& cache = getCache<T>();
		AT_CORE_ASSERT(cache.contains(key), "Asset not found! ({})", key);
		return cache[key];
	}
	template <typename T>
	static bool exists(const std::string& key) {
		return getCache<T>().contains(key);
	}

  private:
	template <typename T>
	static std::unordered_map<std::string, std::shared_ptr<T>>& getCache();
};

template <>
inline std::unordered_map<std::string, std::shared_ptr<Font>>& AssetManager::getCache<Font>() {
	static std::unordered_map<std::string, std::shared_ptr<Font>> s;
	return s;
}
template <>
inline std::unordered_map<std::string, std::shared_ptr<Shader>>& AssetManager::getCache<Shader>() {
	static std::unordered_map<std::string, std::shared_ptr<Shader>> s;
	return s;
}
template <>
inline std::unordered_map<std::string, std::shared_ptr<Texture>>& AssetManager::getCache<Texture>() {
	static std::unordered_map<std::string, std::shared_ptr<Texture>> s;
	return s;
}
template <>
inline std::unordered_map<std::string, std::shared_ptr<TextureSheet>>& AssetManager::getCache<TextureSheet>() {
	static std::unordered_map<std::string, std::shared_ptr<TextureSheet>> s;
	return s;
}

}  // namespace Atlas