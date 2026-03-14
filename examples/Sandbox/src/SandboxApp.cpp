#include <Atlas.h>

#include "Atlas/ImGui/ImGuiSink.h"
#include "SandboxScene.h"
#include "imgui/imgui.h"

class SandboxLayer : public Atlas::Layer {
  public:
	SandboxLayer() : Layer("Example") {
		m_scene = std::make_shared<SandboxScene>();
		m_sink = Atlas::Log::getImGuiLogger();
		m_rawSink = Atlas::Log::getImGuiRawLogger();
	}

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(Atlas::DeltaTime dt) override {
		m_scene->onUpdate(dt);
	}
	virtual void onEvent(Atlas::Event& event) override {
		m_scene->dispatchEvent(event);
	}

	virtual void onImGuiRender() override {
		auto fbuf = Atlas::Application::get().getFramebuffer();
		void* data = fbuf->getColorTexture(0);
		ImGui::Begin("Viewport");

		drawImGuiLogWindow();

		ImGui::Image(data, ImVec2(fbuf->getWidth() * 3.0 / 4.0, fbuf->getHeight() * 3.0 / 4.0), ImVec2(0, 0), ImVec2(1, 1));
		ImGui::End();
	}

  private:
	std::shared_ptr<SandboxScene> m_scene;
    std::shared_ptr<Atlas::ImGuiSink> m_sink;
    std::shared_ptr<Atlas::ImGuiSink> m_rawSink;

	void drawImGuiLogWindow() {
		ImGui::Begin("Log");

		if (ImGui::Button("Clear")) {
			m_sink->clear();
			m_rawSink->clear();
		}

		ImGui::Separator();

		ImGui::BeginChild("Scrolling");
		
		for (const auto& message : m_rawSink->getMessages()) {
			ImVec4 color;
			color.x = message.color.x;
			color.y = message.color.y;
			color.z = message.color.z;
			color.w = message.color.w;
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(message.text.c_str());
			ImGui::PopStyleColor();
		}
		for (const auto& message : m_sink->getMessages()) {
			ImVec4 color;
			color.x = message.color.x;
			color.y = message.color.y;
			color.z = message.color.z;
			color.w = message.color.w;
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(message.text.c_str());
			ImGui::PopStyleColor();
		}

		if (m_rawSink->scrollsToBottom()) {
			ImGui::SetScrollHereY(1.0f);
			m_rawSink->clearScrollFlag();
		}

		if (m_sink->scrollsToBottom()) {
			ImGui::SetScrollHereY(1.0f);
			m_sink->clearScrollFlag();
		}

		ImGui::EndChild();
		ImGui::End();
	}
};

class SandboxApp : public Atlas::Application {
  public:
	SandboxApp() {
		pushLayer(new SandboxLayer());
	}
};

Atlas::Application* Atlas::createApplication() {
	return new SandboxApp;
}