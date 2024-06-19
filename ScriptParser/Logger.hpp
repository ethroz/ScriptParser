#pragma once

#include <cstdint>
#include <iostream>

#include "Other.hpp"

class LogObject {
public:
    ~LogObject() { if (m_endl) m_out << '\n'; if (m_flush) m_out << std::flush; }

    template <typename T>
    const LogObject& operator<<(const T& val) const { m_out << val; return *this; }

private:
    friend class Logger;
    explicit LogObject(std::ostream& stream, bool endl, bool flush) : m_out(stream), m_endl(endl), m_flush(flush) {}

    std::ostream& m_out;
    bool m_endl;
    bool m_flush;
};

enum class LogLevel : uint8_t {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    FATAL = 5
};

class Logger {
public:
    static Logger& getInstance(std::ostream& outStream = std::cout, std::ostream& errStream = std::cerr) {
        static Logger instance(outStream, errStream);
        return instance;
    }

    void logLevel(LogLevel level) { m_level = level; }
    LogLevel logLevel() const { return m_level; }

    LogObject fatal(bool endl = true, bool flush = true) { return LogObject(m_level <= LogLevel::FATAL ? m_err : m_sink, endl, flush); }
    LogObject error(bool endl = true, bool flush = true) { return LogObject(m_level <= LogLevel::ERROR ? m_err : m_sink, endl, flush); }
    LogObject warning(bool endl = true, bool flush = true) { return LogObject(m_level <= LogLevel::WARNING ? m_out : m_sink, endl, flush); }
    LogObject info(bool endl = true, bool flush = true) { return LogObject(m_level <= LogLevel::INFO ? m_out : m_sink, endl, flush); }
    LogObject debug(bool endl = true, bool flush = true) { return LogObject(m_level <= LogLevel::DEBUG ? m_out : m_sink, endl, flush); }
    LogObject trace(bool endl = true, bool flush = true) { return LogObject(m_level <= LogLevel::TRACE ? m_out : m_sink, endl, flush); }

private:
    Logger(std::ostream& outStream, std::ostream& errStream) : m_out(outStream), m_err(errStream) {}
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ostream m_sink = std::ostream(nullptr);
    std::ostream& m_out;
    std::ostream& m_err;
    LogLevel m_level = LogLevel::INFO;
};

static Logger& logger = Logger::getInstance();
