#include "atpch.h"
#include "TextureSheet.h"

namespace Atlas {

TextureSheet::TextureSheet(const std::string& filepath, const TextureSheetSpecification& specs) : m_sheetSpecs(specs) {
    AT_PROFILE_FUNCTION();

    m_sheet = Texture::create(filepath);
    m_sheetSize = glm::vec2(m_sheet->getWidth(), m_sheet->getHeight());
    m_tileDims = m_sheetSize / m_sheetSpecs.tileSize;
}

TextureSheet::TextureSheet(std::shared_ptr<Texture> textureSheet, const TextureSheetSpecification& specs) : m_sheet(textureSheet), m_sheetSpecs(specs) {
    AT_PROFILE_FUNCTION();

    m_sheetSize = glm::vec2(textureSheet->getWidth(), textureSheet->getHeight());
    m_tileDims = m_sheetSize / m_sheetSpecs.tileSize;
}

std::shared_ptr<SubTexture> TextureSheet::getSubTexture(const std::string& name) {
    AT_CORE_ASSERT(m_atlas.contains(name), "SubTexture [{}] does not exist!");

    return m_atlas[name];
}

std::shared_ptr<SubTexture> TextureSheet::addSubTexture(const std::string& name, glm::ivec2 index, glm::ivec2 size_in_tiles) {
    AT_PROFILE_FUNCTION();

    if (m_atlas.contains(name)) {  // subtexture has been cached
        return m_atlas[name];
    }

    std::shared_ptr<SubTexture> newTexture = createSubTexture(index, size_in_tiles);
    m_atlas[name] = newTexture;

    return newTexture;
}

std::shared_ptr<SubTexture> TextureSheet::createSubTexture(glm::ivec2 index, glm::ivec2 size_in_tiles) {
    AT_PROFILE_FUNCTION();

    float textureWidth = size_in_tiles.x * m_sheetSpecs.tileSize.x;
    float textureHeight = size_in_tiles.y * m_sheetSpecs.tileSize.y;

    TextureCoordinates coords;
    coords.top_left = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x, 
            index.y * m_sheetSpecs.tileSize.y
        );
    coords.top_right = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x + textureWidth, 
            index.y * m_sheetSpecs.tileSize.y
        );
    coords.bottom_left = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x, 
            index.y * m_sheetSpecs.tileSize.y + textureHeight
        );
    coords.bottom_right = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x + textureWidth, 
            index.y * m_sheetSpecs.tileSize.y + textureHeight
        );

    coords.top_left.x *= (1.0f / m_sheetSize.x);
    coords.top_left.y *= (1.0f / m_sheetSize.y);
    coords.top_right.x *= (1.0f / m_sheetSize.x);
    coords.top_right.y *= (1.0f / m_sheetSize.y);
    coords.bottom_left.x *= (1.0f / m_sheetSize.x);
    coords.bottom_left.y *= (1.0f / m_sheetSize.y);
    coords.bottom_right.x *= (1.0f / m_sheetSize.x);
    coords.bottom_right.y *= (1.0f / m_sheetSize.y);

    SubTextureSpecification specs;
    specs.coordinates = coords;
    specs.index = index;
    specs.tileDims = size_in_tiles;

    return std::make_shared<SubTexture>(m_sheet, specs);
}

} // namespace Atlas