#include "Timer.hpp"

using namespace std::chrono_literals;

Timer::Timer(bool start) {
    if (start) {
        this->start();
    }
}

void Timer::start() {
    m_running = true;
    reset();
}

void Timer::reset() {
    m_timeStart = Clock::now();
}

void Timer::stop() {
    m_running = false;
    m_timeStop = Clock::now();
}

Duration Timer::elapsed() const {
    Clock::time_point timeStop = m_running ? Clock::now() : m_timeStop;
    return Duration(timeStop - m_timeStart);
}