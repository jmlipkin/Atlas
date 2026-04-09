#pragma once

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/ImGui/EditorWidgets.h"

#include <imgui/imgui.h>

namespace Atlas {

class RenderStatsPanel {
  public:
	RenderStatsPanel() : m_stats(Renderer::getStats()) {}

	void open() {
		m_isOpen = true;
	}

	// Used for Editor Panel
	void onImGuiRender() {
		updateSmoothedTime();

		ImGui::Begin("Render Stats", &m_isOpen);

		ImGui::Text("Frame Time (ms): %.2lf", m_smoothFrameTime);
		ImGui::Text("FPS: %.2lf", m_smoothFps);

		ImGui::Text("Draw Calls: %u", m_stats.drawCalls);
		ImGui::Text("Texture Count: %u", m_stats.textureCount);

		ImGui::Text("Index Count: %u", m_stats.indexCount);
		ImGui::Text("Vertex Count: %u", m_stats.vertexCount);

		ImGui::End();
	}

	// Used for Preview overlay
	// Does not obey isOpen
	void onOverlay(ImVec2 viewportPos) {
		updateSmoothedTime();

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;
		ImVec4			 col   = EditorWidgets::purpleLight;

		ImGui::SetNextWindowPos(ImVec2(viewportPos.x + 10, viewportPos.y + 10), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(180, 40), ImGuiCond_Always);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {col.x, col.y, col.z, 0.1});
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGui::Begin("##RenderStatsOverlay", nullptr, flags);

		ImGui::Text("Frame Time (ms): %.2lf", m_smoothFrameTime);
		ImGui::Text("FPS: %.2lf", m_smoothFps);

		ImGui::End();
		ImGui::PopStyleColor(2);
	}

  private:
	void updateSmoothedTime() {
		m_frameSamples[m_sampleIndex] = m_stats.frameTimeMs;
		m_sampleIndex				  = (m_sampleIndex + 1) % k_sampleCount;

		float sum = 0.0f;
		for (float s : m_frameSamples) sum += s;
		m_smoothFrameTime = sum / k_sampleCount;
		m_smoothFps		  = 1000.0f / m_smoothFrameTime;
	}

  private:
	const RenderStats& m_stats;

	bool m_isOpen = false;

	static constexpr size_t			 k_sampleCount = 60;
	std::array<float, k_sampleCount> m_frameSamples{};
	size_t							 m_sampleIndex	   = 0;
	float							 m_smoothFrameTime = 0.0f;
	float							 m_smoothFps	   = 0.0f;
};

}  // namespace Atlas