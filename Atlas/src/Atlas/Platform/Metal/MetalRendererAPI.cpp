#include "atpch.h"
#include "MetalRendererAPI.h"
#include "Atlas/Platform/Metal/MetalFramebuffer.h"
#include "Atlas/Renderer/Framebuffer.h"

#include "MetalBuffer.h"
#include "MetalPipeline.h"
#include "MetalTexture.h"

#include <imgui/imgui.h>
#include "ImGuiMetalRenderer.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

MetalRendererAPI::MetalRendererAPI(MetalContext& context) : m_context(context) {
	AT_PROFILE_FUNCTION();

	m_commandQueue = context.getMTLDevice()->newCommandQueue();
}

MetalRendererAPI::~MetalRendererAPI() {
}

void MetalRendererAPI::setClearColor(const glm::vec4& color) {
	m_color = MTL::ClearColor::Make((double)color.r, (double)color.g, (double)color.b, (double)color.a);
}

void MetalRendererAPI::clear() {
}

void MetalRendererAPI::drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer) {
	AT_PROFILE_FUNCTION();

	std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(indexBuffer);
	MTL::Buffer* buffer = MIB->getBuffer();
	m_encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indexBuffer->getCount(), MTL::IndexTypeUInt32, buffer, 0);
}

void MetalRendererAPI::drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t count) {
	AT_PROFILE_FUNCTION();

	std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(indexBuffer);
	MTL::Buffer* buffer = MIB->getBuffer();
	m_encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, count, MTL::IndexTypeUInt32, buffer, 0);
}

// void MetalRendererAPI::drawPoint(const std::shared_ptr<VertexArray>& vertexArray) {
//     AT_PROFILE_FUNCTION();

//     std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(vertexArray->getIndexBuffer());
//     MTL::Buffer* indexBuffer = MIB->getBuffer();
//     m_encoder->drawIndexedPrimitives(MTL::PrimitiveTypePoint, vertexArray->getIndexBuffer()->getCount(), MTL::IndexTypeUInt32, indexBuffer, 0);
// }

void MetalRendererAPI::beginFrame() {
	AT_PROFILE_FUNCTION();

	m_pool = NS::AutoreleasePool::alloc()->init();

	m_drawable = m_context.getNextDrawable();
	m_buffer = m_commandQueue->commandBuffer();
}

void MetalRendererAPI::beginPass(std::shared_ptr<Framebuffer> framebuffer) {
	m_framebuffer = std::static_pointer_cast<MetalFramebuffer>(framebuffer);
	m_passDesc = (MTL::RenderPassDescriptor*)m_framebuffer->getPassDescriptor(m_drawable);
	m_passDesc->setRenderTargetWidth(m_framebuffer->getWidth());
	m_passDesc->setRenderTargetHeight(m_framebuffer->getHeight());
	m_encoder = m_buffer->renderCommandEncoder(m_passDesc);

	MTL::Viewport viewport{0, 0, (double)m_framebuffer->getWidth(), (double)m_framebuffer->getHeight(), 0.0, 1.0};
	m_encoder->setViewport(viewport);
	m_encoder->setDepthStencilState((MTL::DepthStencilState*)m_framebuffer->getDepthStencilState());
 
	clear();
}

void MetalRendererAPI::bindPipeline(std::shared_ptr<Pipeline> pipeline, const UniformBuffer& uBuffer) {
	AT_PROFILE_FUNCTION();

	std::shared_ptr<MetalPipeline> p = std::static_pointer_cast<MetalPipeline>(pipeline);
	p->attachFramebuffer(m_framebuffer);
	m_encoder->setRenderPipelineState(p->getMTLPSO());
	m_encoder->setVertexBuffer(static_cast<MTL::Buffer*>(uBuffer.getNativeBuffer()), 0, uBuffer.getIndex());
	m_encoder->setFragmentBuffer(static_cast<MTL::Buffer*>(uBuffer.getNativeBuffer()), 0, uBuffer.getIndex());
}

void MetalRendererAPI::bindTexture(const Texture& texture, uint32_t index) {
	AT_PROFILE_FUNCTION();

	const MetalTexture& t = static_cast<const MetalTexture&>(texture);
	m_encoder->setFragmentTexture(t.getMTLTexture(), index);
}

void MetalRendererAPI::bindVertexBuffer(const VertexBuffer& buffer, uint32_t offset, int index) {
	AT_PROFILE_FUNCTION();

	const MetalVertexBuffer& mBuf = static_cast<const MetalVertexBuffer&>(buffer);
	m_encoder->setVertexBuffer((MTL::Buffer*)mBuf.getNativeBuffer(), offset, index);
}

void MetalRendererAPI::endPass() {
	m_encoder->endEncoding();
}

void MetalRendererAPI::endFrame() {
	AT_PROFILE_FUNCTION();

	m_buffer->presentDrawable((MTL::Drawable*)m_drawable);
	m_buffer->commit();
	m_pool->release();
}

void MetalRendererAPI::beginImGui() {
	AT_PROFILE_FUNCTION();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)m_framebuffer->getWidth(), (float)m_framebuffer->getHeight());

	ImGui_ImplMetal_NewFrame(m_passDesc);
	ImGui::NewFrame();
}

void MetalRendererAPI::drawImGui() {
	AT_PROFILE_FUNCTION();

	ImGui::Render();
	ImGui_ImplMetal_RenderDrawData(
		ImGui::GetDrawData(),
		m_buffer,
		m_encoder);
}

}  // namespace Atlas