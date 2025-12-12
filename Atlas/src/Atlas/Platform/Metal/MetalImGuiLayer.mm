#include "atpch.h"
#include "MetalImGuiLayer.h"

#include <imgui/imgui.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <metal-cpp/Metal.hpp>

#include <Atlas/Core.h>
#include <Atlas/Application.h>
#include <Atlas/KeyCodes.h>
#include <Atlas/MouseButtonCodes.h>
#include <Atlas/Platform/Metal/ImGuiMetalRenderer.h>

namespace Atlas {

    MetalImGuiLayer::MetalImGuiLayer() : ImGuiLayer() {
         m_MTLData = MetalContext::getMTLData();
    }

    void MetalImGuiLayer::onAttach() {
        ImGuiContext *ctx = ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();

        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.IniFilename = "build/Atlas/extern/imgui.ini";

        GLFWwindow* window = MetalContext::getMTLData()->glfwWindow;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
		io.DisplaySize = ImVec2((float)width, (float)height);

        ImGui_ImplMetal_Init((id<MTLDevice>)m_MTLData->device);
    }

    void MetalImGuiLayer::onDetach() {

    }

    void MetalImGuiLayer::onUpdate() {
        ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

        CA::MetalDrawable* drawable = m_MTLData->currentDrawable;
        // auto* attachment = m_MTLData->passDesc->colorAttachments()->object(0);
        // attachment->setTexture(drawable->texture());
        // attachment->setLoadAction(MTL::LoadActionClear);
        // attachment->setStoreAction(MTL::StoreActionStore);
        // attachment->setClearColor(MTL::ClearColor(0.0, 0.0, 0.0, 1.0));

        // if (!drawable || !passDesc) {
        //     return;
        // }

        // --- COMMAND BUFFER ---
        MTL::CommandBuffer* cmd = m_MTLData->commandBuffer;

        // --- IMGUI NEW FRAME ---
        // REQUIRED: gives ImGui the render pass descriptor
        ImGui_ImplMetal_NewFrame((MTLRenderPassDescriptor*)m_MTLData->passDesc);
        ImGui::NewFrame();

        // --- EXAMPLE UI ---
        static bool showDemo = true;
        ImGui::ShowDemoWindow(&showDemo);

        // --- RENDER IMGUI ---
        ImGui::Render();

        // --- CREATE RENDER ENCODER ---
        MTL::RenderCommandEncoder* encoder =
            cmd->renderCommandEncoder(m_MTLData->passDesc);

        // --- IMGUI DRAWDATA ---
        ImGui_ImplMetal_RenderDrawData(
            ImGui::GetDrawData(),
            id<MTLCommandBuffer>(cmd),
            id<MTLRenderCommandEncoder>(encoder)
    );

    encoder->endEncoding();

    // --- PRESENT ---
    cmd->presentDrawable(drawable);
    // cmd->commit();
    }

}  // namespace Atlas