#include "atpch.h"
#include "Font.h"
#include <cstdint>
#include <memory>
#include <string>

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Core/FontLibrary.h"

#include "freetype/freetype.h"

namespace Atlas {
Font::Font(const std::string& name, const std::string& filepath, uint32_t fontSizePx) {
	m_name = name;
    FT_Face face;
	int error = FT_New_Face(FontLibrary::getLibrary(), filepath.c_str(), 0, &face);

	AT_CORE_ASSERT(!error, "Could not load font from file: {}", FT_Error_String(error));

	glm::uvec2 resolution = Application::get().getWindow().getResolution();
	FT_Set_Char_Size(face, 0, fontSizePx * 64, resolution.x, resolution.y);

	error = FT_Load_Char(face, 35, FT_LOAD_RENDER);

	FT_ULong charcode;
	FT_UInt gindex;
	Character character;

    uint32_t textureWidth, textureHeight = 0;

	charcode = FT_Get_First_Char(face, &gindex);
	while (gindex != 0) {
		error = FT_Load_Char(face, charcode, FT_LOAD_RENDER);
		if (error)
			AT_CORE_WARN("Could not load char '{:c}': {}", charcode, FT_Error_String(error));

		character.size.x = face->glyph->bitmap.width;
		character.size.y = face->glyph->bitmap.rows;
		character.bearing.x = face->glyph->bitmap_left;
		character.bearing.y = face->glyph->bitmap_top;
		character.advance = face->glyph->advance.x;
		character.data = face->glyph->bitmap.buffer;

        textureWidth += character.size.x;

        if(character.size.y > textureHeight) {
            textureHeight = character.size.y;
        }

		m_characters[charcode] = character;

		charcode = FT_Get_Next_Char(face, charcode, &gindex);
	}

    AT_CORE_DEBUG("{} x {}", textureWidth, textureHeight);

    m_texture = Texture::create(textureWidth, textureHeight);

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

}  // namespace Atlas