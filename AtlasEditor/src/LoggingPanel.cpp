#include "LoggingPanel.h"
#include "Atlas/Core/Log.h"
#include "atpch.h"

#include <imgui/imgui.h>

namespace Atlas {

LoggingPanel::LoggingPanel() {
	m_logger = Atlas::Log::getImGuiLogger();
	m_rawOut = Atlas::Log::getImGuiRawLogger();
}

void LoggingPanel::onImGuiRender() {
	ImGui::Begin("Log");

	if (ImGui::Button("Clear")) {
		m_logger->clear();
		m_rawOut->clear();
	}

	ImGui::Separator();

	ImGui::BeginChild("Scrolling");

	for (const auto& message : m_rawOut->getMessages()) {
		ImVec4 color;
		color.x = message.color.x;
		color.y = message.color.y;
		color.z = message.color.z;
		color.w = message.color.w;
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(message.text.c_str());
		ImGui::PopStyleColor();
	}
	for (const auto& message : m_logger->getMessages()) {
		ImVec4 color;
		color.x = message.color.x;
		color.y = message.color.y;
		color.z = message.color.z;
		color.w = message.color.w;
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(message.text.c_str());
		ImGui::PopStyleColor();
	}

	if (m_rawOut->scrollsToBottom()) {
		ImGui::SetScrollHereY(1.0f);
		m_rawOut->clearScrollFlag();
	}

	if (m_logger->scrollsToBottom()) {
		ImGui::SetScrollHereY(1.0f);
		m_logger->clearScrollFlag();
	}

	ImGui::EndChild();
	ImGui::End();
}

}  // namespace Atlas