#pragma once

#include <string_view>
#include <variant>
#include <vector>

template <typename T> class Expression;

template <typename T> using ExpressionList = std::vector<Expression<T>>;

template <typename T> class Expression {
public:
    template <typename U>
    constexpr Expression(U&& value) noexcept : m_data{std::move(value)} {}

    template <typename U>
    constexpr Expression(const U& value) noexcept : m_data{value} {}

private:
    template <typename U, typename R>
    friend constexpr bool is(const Expression<R>& expr);
    template <typename U, typename R>
    friend constexpr const U& get(const Expression<R>& expr);

    std::variant<T, char, std::string_view, ExpressionList<T>> m_data;
};
static_assert(std::is_destructible_v<Expression<int>>);

template <typename U, typename T> constexpr bool is(const Expression<T>& expr) {
    return std::holds_alternative<U>(expr.m_data);
}

template <typename U, typename T>
constexpr const U& get(const Expression<T>& expr) {
    return std::get<U>(expr.m_data);
}
