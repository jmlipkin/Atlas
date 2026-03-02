#include "atpch.h"
#include "Font.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Core/FontLibrary.h"
#include "Atlas/Renderer/Texture.h"

#include "freetype/freetype.h"

#define MAX_FONTATLAS_WIDTH 4096

namespace Atlas {
Font::Font(const std::string& name, const std::string& filepath, uint32_t fontSizePx) {
	m_name = name;
	FT_Face face;
	int error = FT_New_Face(FontLibrary::getLibrary(), filepath.c_str(), 0, &face);

	AT_CORE_ASSERT(!error, "Could not load font from file: {}", FT_Error_String(error));

	glm::uvec2 resolution = Application::get().getWindow().getResolution();
	// FT_Set_Char_Size(face, 0, fontSizePx * 64, resolution.x, resolution.y);
	FT_Set_Pixel_Sizes(face, 0, fontSizePx);
	
	loadGlyphs(face);
	createFontTextureAtlas();

	FT_Done_Face(face);
}

std::string Font::to_utf8(uint32_t cp) {
	std::string result;

	if (cp <= 0x7F)
		result += static_cast<char>(cp);
	else if (cp <= 0x7FF) {
		result += static_cast<char>(0xC0 | (cp >> 6));
		result += static_cast<char>(0x80 | (cp & 0x3F));
	} else if (cp <= 0xFFFF) {
		result += static_cast<char>(0xE0 | (cp >> 12));
		result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		result += static_cast<char>(0x80 | (cp & 0x3F));
	} else {
		result += static_cast<char>(0xF0 | (cp >> 18));
		result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
		result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		result += static_cast<char>(0x80 | (cp & 0x3F));
	}

	return result;
}

void Font::loadGlyphs(FT_Face face) {
	FT_ULong charcode;
	FT_UInt gindex;
	Character character;
	int error = 0;

	charcode = FT_Get_First_Char(face, &gindex);
	while (gindex != 0) {
		error = FT_Load_Char(face, charcode, FT_LOAD_RENDER);
		if (error)
			AT_CORE_WARN("Could not load char '{:c}': {}", charcode, FT_Error_String(error));

		const FT_Bitmap& bitmap = face->glyph->bitmap;

		character.size.x = bitmap.width;
		character.size.y = bitmap.rows;
		character.bearing.x = face->glyph->bitmap_left;
		character.bearing.y = face->glyph->bitmap_top;
		character.advance = face->glyph->advance.x;

		character.data.resize(character.size.x * character.size.y);
		for (uint32_t row = 0; row < bitmap.rows; row++) {
			std::memcpy(
				character.data.data() + row * bitmap.width,
				bitmap.buffer + row * bitmap.pitch,
				bitmap.width);
		}

		m_characters[charcode] = character;
		m_orderedGlyphs.push_back(charcode);

		charcode = FT_Get_Next_Char(face, charcode, &gindex);
	}
}

void Font::createFontTextureAtlas() {
	uint32_t currentX = 0;
	uint32_t currentY = 0;
	uint32_t rowHeight = 0;

	uint32_t atlasWidth = 0;
	uint32_t atlasHeight = 0;

	for (auto charcode : m_orderedGlyphs) {
		Character& c = m_characters[charcode];

		// Wrap to next row if needed
		if (currentX + c.size.x > MAX_FONTATLAS_WIDTH) {
			currentX = 0;
			currentY += rowHeight;
			rowHeight = 0;
		}

		c.texOffset.x = currentX;
		c.texOffset.y = currentY;

		currentX += c.size.x;

		if (c.size.y > rowHeight)
			rowHeight = c.size.y;

		atlasWidth = std::max(atlasWidth, currentX);
	}

	atlasHeight = currentY + rowHeight;

	std::vector<unsigned char> atlasData(
		atlasWidth * atlasHeight,
		0);

	for (auto charcode : m_orderedGlyphs) {
		const Character& c = m_characters[charcode];

		for (uint32_t row = 0; row < c.size.y; row++) {
			const unsigned char* src =
				c.data.data() + row * c.size.x;

			unsigned char* dst =
				atlasData.data() + (c.texOffset.y + row) * atlasWidth + c.texOffset.x;

			memcpy(dst, src, c.size.x);
		}
	}

	float epsilonOffset = 0.0002;
	for (auto charcode : m_orderedGlyphs) {
		Character& c = m_characters[charcode];

		c.texCoords.top_left = {
			(float)c.texOffset.x / atlasWidth + epsilonOffset,
			(float)c.texOffset.y / atlasHeight};

		c.texCoords.top_right = {
			(float)(c.texOffset.x + c.size.x) / atlasWidth - epsilonOffset,
			(float)c.texOffset.y / atlasHeight};

		c.texCoords.bottom_left = {
			(float)c.texOffset.x / atlasWidth + epsilonOffset,
			(float)(c.texOffset.y + c.size.y) / atlasHeight};

		c.texCoords.bottom_right = {
			(float)(c.texOffset.x + c.size.x) / atlasWidth - epsilonOffset,
			(float)(c.texOffset.y + c.size.y) / atlasHeight};
	}

	m_texture = Texture::create(
		{atlasWidth, atlasHeight, ImageFormat::R8},
		atlasData.data());
}

}  // namespace Atlas