#pragma once

#include "Atlas/Core/Time.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <thread>

namespace Atlas {
struct ProfileResult {
    std::string name;
    DurationMicro start;
    std::chrono::microseconds elapsedTime;
    uint32_t threadID;
};

struct InstrumentationSession {
    std::string name;
};

class Instrumentor {
   private:
    InstrumentationSession* m_currentSession;
    std::ofstream m_outputStream;
    int m_profileCount;

   public:
    Instrumentor()
        : m_currentSession(nullptr), m_profileCount(0) {
    }

    void beginSession(const std::string& name, const std::string& filepath = "results.json") {
        m_outputStream.open(filepath);
        writeHeader();
        m_currentSession = new InstrumentationSession{name};
    }

    void endSession() {
        writeFooter();
        m_outputStream.close();
        delete m_currentSession;
        m_currentSession = nullptr;
        m_profileCount = 0;
    }

    void writeProfile(const ProfileResult& result) {
        if (m_profileCount++ > 0)
            m_outputStream << ",";

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_outputStream << std::setprecision(3) << std::fixed;
        m_outputStream << "{";
        m_outputStream << "\"cat\":\"function\",";
        m_outputStream << "\"dur\":" << (result.elapsedTime.count()) << ',';
        m_outputStream << "\"name\":\"" << name << "\",";
        m_outputStream << "\"ph\":\"X\",";
        m_outputStream << "\"pid\":0,";
        m_outputStream << "\"tid\":" << result.threadID << ",";
        m_outputStream << "\"ts\":" << result.start.count();
        m_outputStream << "}";

        m_outputStream.flush();
    }

    void writeHeader() {
        m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
        m_outputStream.flush();
    }

    void writeFooter() {
        m_outputStream << "]}";
        m_outputStream.flush();
    }

    static Instrumentor& get() {
        static Instrumentor instance;
        return instance;
    }
};

class InstrumentationTimer {
   public:
    InstrumentationTimer(const char* name)
        : m_name(name), m_stopped(false) {
        m_startTimePoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer() {
        if (!m_stopped)
            stop();
    }

    void stop() {
        	auto endTimepoint = Clock::now();
			auto highResStart = DurationMicro{ m_startTimePoint.time_since_epoch() };
			auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::get().writeProfile({m_name, highResStart, elapsedTime, threadID});

        m_stopped = true;
    }

   private:
    const char* m_name;
    TimePoint m_startTimePoint;
    bool m_stopped;
};
}  // namespace Atlas

#define AT_PROFILE 1
#if AT_PROFILE

#if defined(_MSC_VER)
    #define AT_FUNC_SIG __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
    #define AT_FUNC_SIG __PRETTY_FUNCTION__
#else
    #define AT_FUNC_SIG __func__
#endif


#define AT_PROFILE_BEGIN_SESSION(name, filepath) ::Atlas::Instrumentor::get().beginSession(name, filepath)
#define AT_PROFILE_END_SESSION() ::Atlas::Instrumentor::get().endSession()
#define AT_PROFILE_SCOPE(name) ::Atlas::InstrumentationTimer timer##__LINE__(name);
#define AT_PROFILE_FUNCTION() AT_PROFILE_SCOPE(AT_FUNC_SIG)
#else
#define AT_PROFILE_BEGIN_SESSION(name, filepath)
#define AT_PROFILE_END_SESSION()
#define AT_PROFILE_SCOPE(name)
#define AT_PROFILE_FUNCTION()
#endif