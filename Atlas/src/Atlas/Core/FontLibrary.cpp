#include "Atlas/Core/Font.h"
#include "atpch.h"
#include "FontLibrary.h"

#include <freetype/freetype.h>

namespace Atlas {

    FT_Library FontLibrary::s_library = NULL;
    std::unordered_map<std::string, std::shared_ptr<Font>> FontLibrary::s_fonts;

    void FontLibrary::init() {
        int error = FT_Init_FreeType(&s_library);
        AT_ASSERT(!error, "Error initializing FreeType library: {}", FT_Error_String(error));

        AT_CORE_TRACE("FontLibrary initialized!");
    }

    void FontLibrary::add(std::string name, const std::shared_ptr<Font>& font) {
        AT_PROFILE_FUNCTION();
        
        AT_CORE_ASSERT(!exists(name), "Font already exists!");
        s_fonts[name] = font;
    }

    void FontLibrary::add(const std::shared_ptr<Font>& font) {
        AT_PROFILE_FUNCTION();

        const std::string& name = font->getName();
        AT_CORE_ASSERT(!name.empty(), "Font must have a name!");
        add(name, font);
    }

    std::shared_ptr<Font> FontLibrary::load(const std::string &name, const std::string &filepath) {
        AT_PROFILE_FUNCTION();

        std::shared_ptr<Font> font = std::make_shared<Font>(name, filepath);
        add(name, font);
        return font;
    }

    std::shared_ptr<Font> FontLibrary::get(const std::string &name) {
        AT_PROFILE_FUNCTION();

        AT_CORE_ASSERT(exists(name), "Font does not exist!");
        return s_fonts.at(name);
    }

    bool FontLibrary::exists(const std::string& name) {
        AT_PROFILE_FUNCTION();

        return s_fonts.find(name) != s_fonts.end();
    }
}