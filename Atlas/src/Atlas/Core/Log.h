#pragma once

#include "Base.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Atlas/ImGui/ImGuiSink.h"
#include "spdlog/logger.h"

namespace Atlas {

class Log {
   public:
    static void init();

    inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
    inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; }
    inline static std::shared_ptr<spdlog::logger>& getRawLogger() { return s_rawLogger; }

    inline static std::shared_ptr<ImGuiSink>& getImGuiLogger() { return s_ImGuiSink; }
    inline static std::shared_ptr<ImGuiSink>& getImGuiRawLogger() { return s_ImGuiRawSink; }

   private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
	static std::shared_ptr<spdlog::logger> s_clientLogger;
	static std::shared_ptr<spdlog::logger> s_rawLogger;
	static std::shared_ptr<ImGuiSink> s_ImGuiSink;
	static std::shared_ptr<ImGuiSink> s_ImGuiRawSink;
};
}  // namespace Atlas

#define AT_LOG(...) ::Atlas::Log::getRawLogger()->trace(__VA_ARGS__)
// Core log macros
#define AT_CORE_TRACE(...) ::Atlas::Log::getCoreLogger()->trace(__VA_ARGS__)
#define AT_CORE_DEBUG(...) ::Atlas::Log::getCoreLogger()->debug(__VA_ARGS__)
#define AT_CORE_INFO(...) ::Atlas::Log::getCoreLogger()->info(__VA_ARGS__)
#define AT_CORE_WARN(...) ::Atlas::Log::getCoreLogger()->warn(__VA_ARGS__)
#define AT_CORE_ERROR(...) ::Atlas::Log::getCoreLogger()->error(__VA_ARGS__)
#define AT_CORE_CRITICAL(...) ::Atlas::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define AT_TRACE(...) ::Atlas::Log::getClientLogger()->trace(__VA_ARGS__)
#define AT_DEBUG(...) ::Atlas::Log::getClientLogger()->debug(__VA_ARGS__)
#define AT_INFO(...) ::Atlas::Log::getClientLogger()->info(__VA_ARGS__)
#define AT_WARN(...) ::Atlas::Log::getClientLogger()->warn(__VA_ARGS__)
#define AT_ERROR(...) ::Atlas::Log::getClientLogger()->error(__VA_ARGS__)
#define AT_CRITICAL(...) ::Atlas::Log::getClientLogger()->critical(__VA_ARGS__)