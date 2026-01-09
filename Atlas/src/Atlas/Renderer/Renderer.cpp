#include "Renderer.h"
#include "atpch.h"

#include <glm/glm.hpp>

namespace Atlas {

struct RendererData {

    std::shared_ptr<VertexArray> quadVertexArray;
    std::shared_ptr<Pipeline> quadPipeline;
    std::shared_ptr<UniformBuffer> quadUniforms;

    std::shared_ptr<ShaderLibrary> shaderLib;
};

static RendererData s_data;

void Renderer::init(GraphicsContext& context) {
    RenderCommand::init(context);
    
    float quadVertices[4 * 3] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    
    s_data.quadVertexArray.reset(VertexArray::create());
    std::shared_ptr<VertexBuffer> quadVB = VertexBuffer::create(quadVertices, sizeof(quadVertices));
    s_data.quadVertexArray->addVertexBuffer(quadVB);

    uint32_t quadIndices[6] = {0, 1, 2, 0, 2, 3};
    std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
    s_data.quadVertexArray->setIndexBuffer(quadIB);

    std::string filepath = "/Users/jared/Documents/GameDev/Atlas/examples/PacMan/src/shaders.metallib";
    s_data.shaderLib = ShaderLibrary::create(filepath);
    s_data.shaderLib->load("Quad Shader", "quadVertexShader", "quadFragmentShader");

    PipelineSpecification testPipeSpecs;
    testPipeSpecs.name = "Test Pipeline";
    testPipeSpecs.shader = s_data.shaderLib->get("Quad Shader");
    testPipeSpecs.layout = BufferLayout({{"a_Position", ShaderDataType::Float3}});

    s_data.quadUniforms = UniformBuffer::create(
        testPipeSpecs,
        {{"u_color", 0, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}}},
        1
    );
    s_data.quadPipeline = Pipeline::create(testPipeSpecs);

    AT_CORE_TRACE("Renderer initialized");
}

void Renderer::beginScene() {
}

void Renderer::endScene() {
}

void Renderer::beginImGui() {
    RenderCommand::beginImGui();
}

void Renderer::submitImGui() {
    RenderCommand::drawImGui();
}

void Renderer::submit(const Pipeline& pipeline, const std::shared_ptr<VertexArray>& vertexArray, const UniformBuffer& uBuffer) {
    RenderCommand::bindPipeline(pipeline, uBuffer);
    RenderCommand::bindVertexArray(*vertexArray);
    RenderCommand::drawIndexed(vertexArray);
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    drawQuad(glm::vec3(position, 0.0f), size, color);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
    RenderCommand::bindPipeline(*s_data.quadPipeline, *s_data.quadUniforms);
    RenderCommand::bindVertexArray(*s_data.quadVertexArray);

    s_data.quadUniforms->setFloat4("u_color", color);
    RenderCommand::drawIndexed(s_data.quadVertexArray);
}
}  // namespace Atlas