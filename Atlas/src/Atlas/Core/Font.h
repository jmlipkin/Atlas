#pragma once

#include "Atlas/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <freetype/freetype.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace Atlas {
class Font {
  public:
	struct Character {
		std::vector<unsigned char> data;
		glm::ivec2 size;
		glm::ivec2 bearing;
		unsigned int advance;

		glm::ivec2 texOffset;
		glm::vec2 uvMin;
		glm::vec2 uvMax;
	};

  public:
	Font(const std::string& name, const std::string& filepath, uint32_t fontSizePx = 12);

	const std::shared_ptr<Texture>& getTexture() const { return m_texture; }
	const std::string& getName() const { return m_name; }

	static std::string to_utf8(uint32_t charcode);

  private:
	void loadGlyphs(FT_Face face);
	void createFontTextureAtlas();

  private:
	std::unordered_map<unsigned long, Character> m_characters;
	std::vector<unsigned long> m_orderedGlyphs;

	std::shared_ptr<Texture> m_texture;
	std::string m_name;
};

}  // namespace Atlas