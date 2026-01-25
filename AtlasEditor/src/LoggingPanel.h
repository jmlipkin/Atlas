#pragma once

#include "Atlas/ImGui/ImGuiSink.h"

namespace Atlas {

class LoggingPanel {
  public:
	LoggingPanel();
	~LoggingPanel() = default;

	void onImGuiRender();

  private:
	std::shared_ptr<ImGuiSink> m_logger;
	std::shared_ptr<ImGuiSink> m_rawOut;
	
};

}  // namespace Atlas