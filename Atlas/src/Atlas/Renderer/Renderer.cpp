#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "atpch.h"

namespace Atlas {

struct QuadVertex {
    glm::vec3 position;
    glm::vec4 color;

    uint32_t texIndex;
    glm::vec2 texCoord;
};

struct RendererData {
    const uint32_t maxQuadCount = 10000;
    const uint32_t maxVertexCount = maxQuadCount * 4;
    const uint32_t maxIndexCount = maxQuadCount * 6;
    static const uint32_t maxTextureSlots = 32;

    std::shared_ptr<Pipeline> pointPipeline;
    std::shared_ptr<UniformBuffer> pointUniforms;

    std::shared_ptr<VertexBuffer> quadVertexBuffer;
    std::shared_ptr<IndexBuffer> quadIndexBuffer;
    std::shared_ptr<Pipeline> quadPipeline;
    std::shared_ptr<UniformBuffer> quadUniforms;

    std::shared_ptr<ShaderLibrary> shaderLib;

    std::array<std::shared_ptr<Texture>, maxTextureSlots> textureSlots;
    uint32_t textureSlotIndex = 1;  // 0 = white texture, 1 = invalid texture

    uint32_t quadIndexCount = 0;
    QuadVertex* quadVertexBufferBase = nullptr;
    QuadVertex* quadVertexPtr = nullptr;
};

static RendererData s_data;

void Renderer::init(GraphicsContext& context) {
    AT_PROFILE_FUNCTION();

    RenderCommand::init(context);

    std::string filepath = "/Users/jared/Documents/GameDev/Atlas/examples/PacMan/src/shaders.metallib";
    s_data.shaderLib = ShaderLibrary::create(filepath);
    s_data.shaderLib->load("Quad Shader", "quadVertexShader", "quadFragmentShader");
    
#ifdef POINT_BATCHING_NOT_IMPLEMENTED_YET
    s_data.shaderLib->load("Point Shader", "pointVertexShader", "pointFragmentShader");
    float pointVertices[1 * 1] = {
        // 0.0f, 0.0f, 0.0f, // position
        20.0f  // size
    };
    s_data.pointVertexArray.reset(VertexArray::create());
    std::shared_ptr<VertexBuffer> pointVB = VertexBuffer::create(pointVertices, sizeof(pointVertices));
    s_data.pointVertexArray->addVertexBuffer(pointVB);

    uint32_t pointIndices[1] = {0};
    std::shared_ptr<IndexBuffer> pointIB = IndexBuffer::create(pointIndices, sizeof(pointIndices));
    s_data.pointVertexArray->setIndexBuffer(pointIB);

    PipelineSpecification pointPipelineSpecs;
    pointPipelineSpecs.name = "Point Pipeline";
    pointPipelineSpecs.shader = s_data.shaderLib->get("Point Shader");
    pointPipelineSpecs.layout = BufferLayout({/*{"a_Position", ShaderDataType::Float3}, */ {"a_Size", ShaderDataType::Float}});

    s_data.pointUniforms = UniformBuffer::create(
        pointPipelineSpecs, {{"u_position", 0, glm::vec3(0.0f)}, {"u_viewProjection", 1, glm::mat4(1.0f)}, {"u_color", 2, glm::vec4(1.0f)}},
        1);
    s_data.pointPipeline = Pipeline::create(pointPipelineSpecs);
    
#endif

    {
        AT_PROFILE_SCOPE("Allocate QuadVertex data");

        s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertexCount];
    }
    
    s_data.quadVertexBuffer = VertexBuffer::create(s_data.maxVertexCount * sizeof(QuadVertex));

    uint32_t* quadIndices = new uint32_t[s_data.maxIndexCount];
    uint32_t offset = 0;

    {
        AT_PROFILE_SCOPE("Create QuadIndex Data");

        for (uint32_t i = 0; i < s_data.maxIndexCount; i += 6) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }
    }

    s_data.quadIndexBuffer = IndexBuffer::create(quadIndices, s_data.maxIndexCount);

    PipelineSpecification quadPipelineSpecs;
    quadPipelineSpecs.name = "Quad Pipeline";
    quadPipelineSpecs.shader = s_data.shaderLib->get("Quad Shader");
    quadPipelineSpecs.layout = BufferLayout({{"a_position", ShaderDataType::Float3},
                                             {"a_color", ShaderDataType::Float4},
                                             {"a_texIndex", ShaderDataType::Uint},
                                             {"a_texCoord", ShaderDataType::Float2}});

    s_data.quadUniforms = UniformBuffer::create(
        quadPipelineSpecs, {{"u_viewProjection", 0, glm::mat4(1.0f)}},
        1);
    s_data.quadPipeline = Pipeline::create(quadPipelineSpecs);

    std::shared_ptr<Texture> whiteTexture = Texture::create(1, 1);
    uint32_t whiteTextureData = 0xFFFFFFFF;
    whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

    s_data.textureSlots[0] = whiteTexture;

    AT_CORE_TRACE("Renderer initialized");
}

void Renderer::shutdown() {
    AT_PROFILE_FUNCTION();

    delete s_data.quadVertexBufferBase;
}

void Renderer::beginScene(const OrthographicCamera& camera) {
    AT_PROFILE_FUNCTION();

    s_data.quadUniforms->setMat4("u_viewProjection", camera.getViewProjectionMatrix());
    startNewBatch();
}

void Renderer::endScene() {
    AT_PROFILE_FUNCTION();

    flush();
}

void Renderer::startNewBatch() {
    AT_PROFILE_FUNCTION();  

    s_data.quadIndexCount = 0;
    s_data.quadVertexPtr = s_data.quadVertexBufferBase;

    s_data.textureSlotIndex = 1;
}

void Renderer::flush() {
    AT_PROFILE_FUNCTION();

    if(s_data.quadIndexCount == 0) {
        return;
    }

    uint32_t dataSize = (uint8_t*)s_data.quadVertexPtr - (uint8_t*)s_data.quadVertexBufferBase;
    s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);

    for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
        RenderCommand::bindTexture(*s_data.quadPipeline, *s_data.textureSlots[i], i);
    }

    RenderCommand::bindPipeline(*s_data.quadPipeline, *s_data.quadUniforms);
    RenderCommand::bindVertexBuffer(*s_data.quadVertexBuffer);
    RenderCommand::drawIndexed(s_data.quadIndexBuffer, s_data.quadIndexCount);

    startNewBatch();
}

void Renderer::beginImGui() {
    AT_PROFILE_FUNCTION();  

    RenderCommand::beginImGui();
}

void Renderer::submitImGui() {
    AT_PROFILE_FUNCTION();

    RenderCommand::drawImGui();
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    drawQuad(glm::vec3(position, 0.0f), size, color);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
    AT_PROFILE_FUNCTION();

    uint32_t texIndex = 0;
    s_data.quadVertexPtr->position = {position.x, position.y + size.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{0, 1};
    s_data.quadVertexPtr++;
    
    s_data.quadVertexPtr->position = {position.x + size.x, position.y + size.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{1, 1};
    s_data.quadVertexPtr++;
    
    s_data.quadVertexPtr->position = {position.x + size.x, position.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{1, 0};
    s_data.quadVertexPtr++;
    
    s_data.quadVertexPtr->position = {position.x, position.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{0, 0};
    s_data.quadVertexPtr++;

    s_data.quadIndexCount += 6;
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture) {
    drawQuad(glm::vec3(position, 0.0f), size, texture);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture) {
    AT_PROFILE_FUNCTION();
    
    glm::vec4 color = glm::vec4(1.0f);
    uint32_t texIndex = 0;

    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
        if(*texture.get() == *s_data.textureSlots[i].get()) {
            texIndex = i;
            break;
        }
    }
    if(texIndex == 0) {
        if(s_data.textureSlotIndex >= s_data.maxTextureSlots) {
            flush();
        }
        
        texIndex = s_data.textureSlotIndex;
        s_data.textureSlots[texIndex] = texture;
        s_data.textureSlotIndex++;
    }

    s_data.quadVertexPtr->position = {position.x, position.y + size.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{0, 1};
    s_data.quadVertexPtr++;
    
    s_data.quadVertexPtr->position = {position.x + size.x, position.y + size.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{1, 1};
    s_data.quadVertexPtr++;
    
    s_data.quadVertexPtr->position = {position.x + size.x, position.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{1, 0};
    s_data.quadVertexPtr++;
    
    s_data.quadVertexPtr->position = {position.x, position.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = glm::vec2{0, 0};
    s_data.quadVertexPtr++;

    s_data.quadIndexCount += 6;
}

void Renderer::drawQuad(const glm::vec2& position, const std::shared_ptr<SubTexture>& texture) {
    drawQuad(glm::vec3(position, 0.0f), texture->getSizeInTiles(), texture);
}

void Renderer::drawQuad(const glm::vec3& position, const std::shared_ptr<SubTexture>& texture) {
    drawQuad(position, texture->getSizeInTiles(), texture);
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture) {
    drawQuad(glm::vec3(position, 0.0f), size, texture);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture) {
    AT_PROFILE_FUNCTION();
    
    glm::vec4 color = glm::vec4(1.0f);
    uint32_t texIndex = 0;

    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
        if(*texture->getTexture() == *s_data.textureSlots[i].get()) {
            texIndex = i;
            break;
        }
    }
    if(texIndex == 0) {
        if(s_data.textureSlotIndex >= s_data.maxTextureSlots) {
            flush();
        }
        
        texIndex = s_data.textureSlotIndex;
        s_data.textureSlots[texIndex] = texture->getTexture();
        s_data.textureSlotIndex++;
    }

    s_data.quadVertexPtr->position = {position.x, position.y + size.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = texture->getTexCoords().bottom_left;
    s_data.quadVertexPtr++;

    s_data.quadVertexPtr->position = {position.x + size.x, position.y + size.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = texture->getTexCoords().bottom_right;
    s_data.quadVertexPtr++;

    s_data.quadVertexPtr->position = {position.x + size.x, position.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = texture->getTexCoords().top_right;
    s_data.quadVertexPtr++;

    s_data.quadVertexPtr->position = {position.x, position.y, position.z};
    s_data.quadVertexPtr->color = color;
    s_data.quadVertexPtr->texIndex = texIndex;
    s_data.quadVertexPtr->texCoord = texture->getTexCoords().top_left;
    s_data.quadVertexPtr++;

    s_data.quadIndexCount += 6;
}

}  // namespace Atlas