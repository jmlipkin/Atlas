#pragma once

#include <spdlog/sinks/base_sink.h>
#include <glm/glm.hpp>

#include <mutex>
#include <vector>

namespace Atlas {

struct ImGuiLogMessage {
    spdlog::level::level_enum level;
    std::string text;
    glm::vec4 color;
};

class ImGuiSink final : public spdlog::sinks::base_sink<std::mutex> {
   public:
    const std::vector<ImGuiLogMessage>& getMessages() const { return m_messages; }

    void clear() {
        std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);
        m_messages.clear();
    }

    bool scrollsToBottom() const { return m_scrollToBottom; }
    void clearScrollFlag() { m_scrollToBottom = false; }

   protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        base_sink<std::mutex>::formatter_->format(msg, formatted);

        ImGuiLogMessage logMsg;
        logMsg.text = fmt::to_string(formatted);
        logMsg.level = msg.level;
        logMsg.color = computeColor(msg.level);

        m_messages.push_back(logMsg);

        m_scrollToBottom = true;
    }

    void flush_() override {}

   private:
    glm::vec4 computeColor(spdlog::level::level_enum level) {
        using LVL = spdlog::level::level_enum;
        switch (level) {
            case LVL::trace: return {1, 1, 1, 1};
            case LVL::debug: return {0.6f, 0.8f, 1.0f, 1};
            case LVL::info:  return {0.1f, 0.75f, 0.1f, 1};
            case LVL::warn:  return {1, 1, 0, 1};
            case LVL::err:   return {1, 0, 0, 1};
            case LVL::critical: return {1, 0, 1, 1};
            default:
                return {1, 1, 1, 1};
        }
    }
   private:
    std::vector<ImGuiLogMessage> m_messages;
    bool m_scrollToBottom = false;
};

}  // namespace Atlas