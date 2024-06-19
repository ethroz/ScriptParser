#pragma once

#include <concepts>
#include <string_view>
#include <type_traits>

using namespace std::string_view_literals;

template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template<typename T, typename... Ts>
struct are_all_same;

template<typename T>
struct are_all_same<T> : std::true_type {};

template<typename T, typename U, typename... Ts>
struct are_all_same<T, U, Ts...> : std::false_type {};

template<typename T, typename... Ts>
struct are_all_same<T, T, Ts...> : are_all_same<T, Ts...> {};

template<typename T, typename... Ts>
constexpr bool are_all_same_v = are_all_same<T, Ts...>::value;
static_assert(are_all_same_v<int>);
static_assert(are_all_same_v<int, int>);
static_assert(are_all_same_v<int, int, int>);
static_assert(are_all_same_v<int, int, int, int>);
static_assert(!are_all_same_v<int, int, int, int, double>);

constexpr std::string_view plural(size_t n) {
    return n == 1 ? ""sv : "s"sv;
}

constexpr bool isAlpha(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

constexpr bool isDigit(char c) { return (c >= '0' && c <= '9'); }

constexpr bool isAlnum(char c) {
    return isAlpha(c) || isDigit(c) || (c == '_');
}

constexpr bool isOp(char c) {
    return (c == '+') || (c == '-') || (c == '*') || (c == '/');
}

constexpr bool isFloat(char c) {
    return isDigit(c) || (c == '+') || (c == '-') || (c == '.');
}

constexpr bool isSpace(char c) {
    return (c == ' ') || (c == '\n');
}
