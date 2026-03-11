#pragma once

#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/TextureSheet.h"

#include <glm/glm.hpp>
#include <freetype/freetype.h>
#include <vector>

namespace Atlas {
class Font {
  public:
	struct Character {
		std::vector<unsigned char> data;
		glm::ivec2 size;
		glm::ivec2 bearing;
		unsigned int advance;

		glm::ivec2 texOffset;
		TextureCoordinates texCoords;
	};

  public:
	Font(const std::string& name, const std::string& filepath, uint32_t fontSizePx = 128);

	const std::shared_ptr<Texture>& getTexture() const { return m_texture; }
	const Character& getCharacter(unsigned long charcode) const { return m_characters.at(charcode); }
	const std::string& getName() const { return m_name; }
	uint32_t getFontSizePx() const { return m_fontSizePx; }

	static std::string to_utf8(uint32_t charcode);

  private:
	void loadGlyphs(FT_Face face);
	void createFontTextureAtlas();

  private:
	std::unordered_map<unsigned long, Character> m_characters;
	std::vector<unsigned long> m_orderedGlyphs;

	std::shared_ptr<Texture> m_texture;
	std::string m_name;
	uint32_t m_fontSizePx;
};

}  // namespace Atlas