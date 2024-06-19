#pragma once

#include <chrono>
#include <string_view>

using namespace std::chrono;
using namespace std::literals;

class Duration {
public:

    Duration() = default;
    Duration(std::string_view str) { set(str); }
    Duration(float value) { set(value); }

    template <class Rep, class Period>
    Duration(duration<Rep, Period> value) { set(value); }

    void set(std::string_view str);
    void set(float value);

    template <class Rep, class Period>
    void set(duration<Rep, Period> value) {
        m_value = duration_cast<nanoseconds>(value);
    }

    nanoseconds ns() const { return m_value; }
    milliseconds ms() const;

    float get() const;

    operator float() const { return get(); }
    operator nanoseconds() const { return ns(); }

private:
    nanoseconds m_value = 0ns;
};