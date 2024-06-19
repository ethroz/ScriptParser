#pragma once

#include <chrono>

#include "Duration.hpp"

class Timer {
public:
    Timer(bool start = false);

    void start();
    void reset();
    void stop();
    Duration elapsed() const;
    bool running() const { return m_running; }

private:
    using Clock = std::chrono::high_resolution_clock;

    Clock::time_point m_timeStart;
    Clock::time_point m_timeStop;
    bool m_running = false;
};