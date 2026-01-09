#include "Renderer.h"
#include "atpch.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Atlas {

struct RendererData {

    std::shared_ptr<VertexArray> pointVertexArray;
    std::shared_ptr<Pipeline> pointPipeline;
    std::shared_ptr<UniformBuffer> pointUniforms;

    std::shared_ptr<VertexArray> quadVertexArray;
    std::shared_ptr<Pipeline> quadPipeline;
    std::shared_ptr<UniformBuffer> quadUniforms;

    std::shared_ptr<ShaderLibrary> shaderLib;
};

static RendererData s_data;

void Renderer::init(GraphicsContext& context) {
    RenderCommand::init(context);

    std::string filepath = "/Users/jared/Documents/GameDev/Atlas/examples/PacMan/src/shaders.metallib";
    s_data.shaderLib = ShaderLibrary::create(filepath);
    s_data.shaderLib->load("Quad Shader", "quadVertexShader", "quadFragmentShader");
    s_data.shaderLib->load("Point Shader", "pointVertexShader", "pointFragmentShader");

    float pointVertices[1 * 1] = {
        // 0.0f, 0.0f, 0.0f, // position
        20.0f // size
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
    pointPipelineSpecs.layout = BufferLayout({ /*{"a_Position", ShaderDataType::Float3}, */{"a_Size", ShaderDataType::Float}});

    s_data.pointUniforms = UniformBuffer::create(
        pointPipelineSpecs, {
            {"u_position", 0, glm::vec3(0.0f)},
            {"u_color", 1, glm::vec4(1.0f)}
        },
        1
    );
    s_data.pointPipeline = Pipeline::create(pointPipelineSpecs);

    float quadVertices[4 * 3] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f};

    s_data.quadVertexArray.reset(VertexArray::create());
    std::shared_ptr<VertexBuffer> quadVB = VertexBuffer::create(quadVertices, sizeof(quadVertices));
    s_data.quadVertexArray->addVertexBuffer(quadVB);

    uint32_t quadIndices[6] = {0, 1, 2, 0, 2, 3};
    std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
    s_data.quadVertexArray->setIndexBuffer(quadIB);

    PipelineSpecification quadPipelineSpecs;
    quadPipelineSpecs.name = "Quad Pipeline";
    quadPipelineSpecs.shader = s_data.shaderLib->get("Quad Shader");
    quadPipelineSpecs.layout = BufferLayout({{"a_Position", ShaderDataType::Float3}});

    s_data.quadUniforms = UniformBuffer::create(
        quadPipelineSpecs, {
            {"u_transform", 0, glm::mat4(0.0f)},
            {"u_color", 1, glm::vec4(1.0f)}
        },
        1
    );
    s_data.quadPipeline = Pipeline::create(quadPipelineSpecs);

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

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    s_data.quadUniforms->setMat4("u_transform", transform);
    s_data.quadUniforms->setFloat4("u_color", color);
    RenderCommand::drawIndexed(s_data.quadVertexArray);

    RenderCommand::bindPipeline(*s_data.pointPipeline, *s_data.pointUniforms);
    RenderCommand::bindVertexArray(*s_data.pointVertexArray);

    glm::vec4 inverse_color = glm::vec4(1.0f - color.r, 1.0f - color.g, 1.0f - color.b, 1.0f);
    s_data.pointUniforms->setFloat3("u_position", position);
    s_data.pointUniforms->setFloat4("u_color", inverse_color);
    RenderCommand::drawPoint(s_data.pointVertexArray);
}
}  // namespace Atlas