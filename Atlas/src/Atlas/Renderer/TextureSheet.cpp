#include "atpch.h"
#include "TextureSheet.h"

namespace Atlas {

TextureSheet::TextureSheet(const std::string& filepath, const TextureSheetSpecification& specs) : m_sheetSpecs(specs) {
    AT_PROFILE_FUNCTION();

    m_sheet = Texture::create(filepath);
    m_sheetSize = glm::vec2(m_sheet->getWidth(), m_sheet->getHeight());
    m_tileDims = m_sheetSize / m_sheetSpecs.tileSize;

    m_coordinates.top_left = glm::vec2{0.0f, 0.0f};
    m_coordinates.top_right = glm::vec2{1.0f, 0.0f};
    m_coordinates.bottom_left = glm::vec2{0.0f, 1.0f};
    m_coordinates.bottom_right = glm::vec2{1.0f, 1.0f};
}

TextureSheet::TextureSheet(std::shared_ptr<Texture> texture, const TextureSheetSpecification& specs) : m_sheet(texture), m_sheetSpecs(specs) {
    AT_PROFILE_FUNCTION();

    m_sheetSize = glm::vec2(texture->getWidth(), texture->getHeight());
    m_tileDims = m_sheetSize / m_sheetSpecs.tileSize;

    m_coordinates.top_left = glm::vec2{0.0f, 0.0f};
    m_coordinates.top_right = glm::vec2{1.0f, 0.0f};
    m_coordinates.bottom_left = glm::vec2{0.0f, 1.0f};
    m_coordinates.bottom_right = glm::vec2{1.0f, 1.0f};
}

TextureSheet::TextureSheet(const TextureSheet& textureSheet, glm::ivec2 start_index, glm::ivec2 end_index) : m_sheet(textureSheet.getTexture()), m_sheetSpecs(textureSheet.getSheetSpecs()) {
    AT_PROFILE_FUNCTION();

    AT_CORE_ASSERT(end_index.x <= textureSheet.getNumTilesHorizontal() && end_index.y <= textureSheet.getNumTilesVertical(), "TextureSheet exceeds texture dimensions!");

    m_tileDims = end_index - start_index;
    m_sheetSize = m_tileDims * m_sheetSpecs.tileSize;

    // Modify "full texture" coordinates
    glm::vec2 fullTileSize = textureSheet.getSizeInTiles();
    m_coordinates.top_left = glm::vec2(start_index) / glm::vec2(fullTileSize);
    m_coordinates.top_right = glm::vec2(end_index.x, start_index.y) / glm::vec2(fullTileSize);
    m_coordinates.bottom_left = glm::vec2(start_index.x, end_index.y) / glm::vec2(fullTileSize);
    m_coordinates.bottom_right = glm::vec2(end_index) / glm::vec2(fullTileSize);
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

    if(index.x + size_in_tiles.x > m_tileDims.x || index.y + size_in_tiles.y > m_tileDims.y)
        AT_CORE_WARN("SubTexture \"{}\" exceeds texture dimensions!", name);

    std::shared_ptr<SubTexture> newTexture = createSubTexture(index, size_in_tiles);
    m_atlas[name] = newTexture;

    return newTexture;
}

std::shared_ptr<SubTexture> TextureSheet::createSubTexture(glm::ivec2 index, glm::ivec2 size_in_tiles) {
    AT_PROFILE_FUNCTION();

    float textureWidth = size_in_tiles.x * m_sheetSpecs.tileSize.x;
    float textureHeight = size_in_tiles.y * m_sheetSpecs.tileSize.y;

    TextureCoordinates local;
    local.top_left = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x, 
            index.y * m_sheetSpecs.tileSize.y
        );
    local.top_right = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x + textureWidth, 
            index.y * m_sheetSpecs.tileSize.y
        );
    local.bottom_left = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x, 
            index.y * m_sheetSpecs.tileSize.y + textureHeight
        );
    local.bottom_right = glm::vec2(
            index.x * m_sheetSpecs.tileSize.x + textureWidth, 
            index.y * m_sheetSpecs.tileSize.y + textureHeight
        );

    local.top_left.x *= (1.0f / m_sheetSize.x);
    local.top_left.y *= (1.0f / m_sheetSize.y);
    local.top_right.x *= (1.0f / m_sheetSize.x);
    local.top_right.y *= (1.0f / m_sheetSize.y);
    local.bottom_left.x *= (1.0f / m_sheetSize.x);
    local.bottom_left.y *= (1.0f / m_sheetSize.y);
    local.bottom_right.x *= (1.0f / m_sheetSize.x);
    local.bottom_right.y *= (1.0f / m_sheetSize.y);

    TextureCoordinates output;

    auto remap = [&](const glm::vec2& uv) {
        return m_coordinates.top_left + uv * (m_coordinates.bottom_right - m_coordinates.top_left);
    };

    output.top_left = remap(local.top_left);
    output.top_right = remap(local.top_right);
    output.bottom_left = remap(local.bottom_left);
    output.bottom_right = remap(local.bottom_right);

    SubTextureSpecification specs;
    specs.coordinates = output;
    specs.index = index;
    specs.tileDims = size_in_tiles;

    return std::make_shared<SubTexture>(m_sheet, specs);
}

} // namespace Atlas