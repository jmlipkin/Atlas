#pragma once

#include "Atlas/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <freetype/freetype.h>
#include <string>
#include <unordered_map>

namespace Atlas {
    class Font {
        public:

        struct Character {
            unsigned char* data;
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };

        public:
        Font(const std::string& name, const std::string& filepath, uint32_t fontSizePx = 12);

        const std::string& getName() const { return m_name; }

        static std::string to_utf8(uint32_t cp);

        private:
        std::string m_name;
        std::unordered_map<unsigned long, Character> m_characters;
        std::shared_ptr<Texture> m_texture;
    };
}