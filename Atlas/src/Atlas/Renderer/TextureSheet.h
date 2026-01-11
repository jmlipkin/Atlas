#pragma once

#include <glm/glm.hpp>

#include "Atlas/Renderer/Texture.h"

namespace Atlas {

struct TextureCoordinates {
    glm::vec2 top_left;
    glm::vec2 top_right;
    glm::vec2 bottom_left;
    glm::vec2 bottom_right;

    bool operator==(const TextureCoordinates& other) const {
        bool equal = true;
        equal &= top_left == other.top_left;
        equal &= top_right == other.top_right;
        equal &= bottom_left == other.bottom_left;
        equal &= bottom_right == other.bottom_right;
        return equal;
    }
};

struct SubTextureSpecification {
    TextureCoordinates coordinates;
    glm::vec2 tileDims;
    glm::ivec2 index;
};

class SubTexture {
   public:
    SubTexture(std::shared_ptr<Texture> texture, const SubTextureSpecification& specs) : m_texture(texture), m_specs(specs) {}
    virtual ~SubTexture() = default;

    const std::shared_ptr<Texture> getTexture() const { return m_texture; }
    const TextureCoordinates& getTexCoords() const { return m_specs.coordinates; }

    float getNumTilesHorizontal() const { return m_specs.tileDims.x; }
    float getNumTilesVertical() const { return m_specs.tileDims.y; }

    glm::vec2 getSizeInTiles() const { return m_specs.tileDims; }

    bool operator==(const SubTexture& other) const { return m_texture == other.m_texture && m_specs.coordinates == other.m_specs.coordinates; }

   private:
    std::shared_ptr<Texture> m_texture;
    SubTextureSpecification m_specs;
};

struct TextureSheetSpecification {
    glm::vec2 tileSize;

    TextureSheetSpecification(glm::vec2 tile_size_pixels = glm::vec2(1.0f)) : tileSize(tile_size_pixels) {}
};

class TextureSheet {
   public:
    TextureSheet(const std::string& filepath, const TextureSheetSpecification& specs);
    TextureSheet(std::shared_ptr<Texture> textureSheet, const TextureSheetSpecification& specs);

    std::shared_ptr<SubTexture> getSubTexture(const std::string& name);
    std::shared_ptr<SubTexture> addSubTexture(const std::string& name, glm::ivec2 index, glm::ivec2 size_in_tiles = glm::ivec2(1));

    float getNumTilesHorizontal() const { return m_tileDims.x; }
    float getNumTilesVertical() const { return m_tileDims.y; }

    glm::vec2 getSizeInTiles() const { return m_tileDims; }
    glm::vec2 getSizeInPixels() const { return m_sheetSize; }

    std::shared_ptr<Texture> getTexture() const { return m_sheet; }

   private:
    std::shared_ptr<SubTexture> createSubTexture(glm::ivec2 index, glm::ivec2 size_in_tiles = glm::vec2(1.0f));

   private:
    std::shared_ptr<Texture> m_sheet;
    std::unordered_map<std::string, std::shared_ptr<SubTexture>> m_atlas;

    glm::vec2 m_sheetSize;
    glm::vec2 m_tileDims;
    const TextureSheetSpecification& m_sheetSpecs;
};

}  // namespace Atlas