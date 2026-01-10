#pragma once

#include <chrono>

namespace Atlas {

typedef std::chrono::steady_clock Clock;
typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> TimePoint;
typedef std::chrono::duration<double> Duration;

class DeltaTime {
   public:
    DeltaTime() : m_lastTimePoint(Clock::now()) {}

    void updateDelta() {
        TimePoint time = Clock::now();
        m_time = Duration(time - m_lastTimePoint).count();
        m_lastTimePoint = time;
    }

    operator double() const { return m_time; }

    double getSeconds() const { return m_time; }
    double getMilliseconds() const { return m_time * 1000; }
    double getMicroseconds() const { return m_time * 1000000; }

   private:
    double m_time{0.0f};

    TimePoint m_lastTimePoint;
};

}  // namespace Atlas