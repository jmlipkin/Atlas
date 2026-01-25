#include "Atlas/ImGui/ImGuiSink.h"
#include "atpch.h"
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>
#include <memory>

// TODO: Move to a config file or CMake
#define AT_ENABLE_IMGUI_LOGGING

namespace Atlas {

    std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;
    std::shared_ptr<ImGuiSink> Log::s_ImGuiSink;

	std::shared_ptr<spdlog::logger> Log::s_rawLogger;
	std::shared_ptr<ImGuiSink> Log::s_ImGuiRawSink;

	void Log::init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        s_coreLogger = spdlog::stdout_color_mt("ATLAS");
        s_coreLogger->set_level(spdlog::level::trace);
        
        s_clientLogger = spdlog::stdout_color_mt("APP");
		s_clientLogger->set_level(spdlog::level::trace);

		s_rawLogger = spdlog::stdout_color_mt("RAW");
		s_rawLogger->set_level(spdlog::level::trace);
		s_rawLogger->set_pattern("%v");

    #ifdef AT_ENABLE_IMGUI_LOGGING
        s_ImGuiSink = std::make_shared<ImGuiSink>();
        s_ImGuiSink->set_level(spdlog::level::trace);
		s_ImGuiSink->set_formatter(std::make_unique<spdlog::pattern_formatter>("[%T] %n: %v"));
        s_coreLogger->sinks().push_back(s_ImGuiSink);
		s_clientLogger->sinks().push_back(s_ImGuiSink);

		s_ImGuiRawSink = std::make_shared<ImGuiSink>();
		s_ImGuiRawSink->set_level(spdlog::level::trace);
		s_ImGuiRawSink->set_formatter(std::make_unique<spdlog::pattern_formatter>("%v"));
		s_rawLogger->sinks().push_back(s_ImGuiRawSink);
    #endif
    }

}  // namespace Atlas