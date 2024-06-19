#include <format>
#include <stdexcept>
#include <string>

#include "Duration.hpp"

using namespace std::chrono;

void Duration::set(std::string_view str) {
    if (str.empty()) {
        throw std::invalid_argument("Unrecognized time: Empty string");
    }

    constexpr float THOUSAND = 1000.0f;
    float scale = 1.0f;

    if (str.ends_with('s')) {
        str.remove_suffix(1);
    }

    if (!str.empty()) {
        switch (str.back()) {
        case 'n':
            scale /= THOUSAND;
            [[fallthrough]];
        case 'u':
            scale /= THOUSAND;
            [[fallthrough]];
        case 'm':
            scale /= THOUSAND;
            str.remove_suffix(1);
        default:
            break;
        }
    }
    else {
        throw std::invalid_argument(std::format("Unrecognized time: {}", str));
    }

    const auto time = std::stof(std::string(str));
    set(time * scale);
}

void Duration::set(float value) {
    m_value = round<nanoseconds>(duration<float>{value});
}

milliseconds Duration::ms() const {
    return round<milliseconds>(m_value);
}

float Duration::get() const {
    return duration_cast<duration<float>>(m_value).count();
}