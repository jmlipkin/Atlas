#include "atpch.h"
#include "AssetManager.h"

namespace Atlas {

std::shared_ptr<Font> AssetManager::loadFont(const std::string& name, const std::string& filepath, uint32_t fontSizePx) {
	auto& cache = getCache<Font>();
	if (cache.contains(name)) {
		return cache[name];
	}
	return cache[name] = std::make_shared<Font>(name, filepath, fontSizePx);
}

std::shared_ptr<Shader> AssetManager::loadShader(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction) {
	auto& cache = getCache<Shader>();
	if (cache.contains(name)) {
		return cache[name];
	}
	return cache[name] = Shader::create(name, vertexFunction, fragmentFunction);
}

std::shared_ptr<Texture> AssetManager::loadTexture(const std::string& filepath) {
	auto& cache = getCache<Texture>();
	if (cache.contains(filepath)) {
		return cache[filepath];
	}
	return cache[filepath] = Texture::create(filepath);
}

std::shared_ptr<TextureSheet> AssetManager::loadTextureSheet(const std::string& filepath, const TextureSheetSpecification& specs) {
	auto& cache = getCache<TextureSheet>();
	if (cache.contains(filepath)) {
		return cache[filepath];
	}
	return cache[filepath] = std::make_shared<TextureSheet>(filepath, specs);
}

}  // namespace Atlas