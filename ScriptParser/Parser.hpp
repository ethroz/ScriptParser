#include <array>
#include <cmath>
#include <format>
#include <functional>
#include <memory>
#include <span>
#include <type_traits>
#include <variant>
#include <vector>

#include "Expression.hpp"
#include "Function.hpp"
#include "Other.hpp"

namespace script {

constexpr size_t opPriority(char op) {
    switch (op) {
    case '+': return 1;
    case '-': return 1;
    case '*': return 2;
    case '/': return 2;
    default: throw std::invalid_argument(std::format("Invalid operator: {}", op));
    }
}
static_assert(opPriority('+') == 1);


template <typename T> constexpr function<T(T, T)> getOpFunc(char op) {
    switch (op) {
    case '+': return [](T left, T right) { return left + right; }; break;
    case '-': return [](T left, T right) { return left - right; }; break;
    case '*': return [](T left, T right) { return left * right; }; break;
    case '/': return [](T left, T right) { return left / right; }; break;
    default: throw std::invalid_argument(std::format("Invalid operator: {}", op));
    }
}
static_assert(getOpFunc<int>('+')(1, 2) == 3);


template<typename T>
struct Operator {
    size_t priority;
    function<T(T, T)> func;
};

template <typename T>
constexpr T parseExpressionTree(std::span<const Expression<T>> expressions) {
    std::vector<T> values;
    std::vector<char> operators;

    for (size_t i = 0; i < expressions.size(); i++) {
        const auto& expr = expressions[i];
        if (is<char>(expr)) {
            char op = get<char>(expr);
            operators.push_back(op);
        }
        else if (is<T>(expr)) {
            T val = get<T>(expr);
            values.push_back(val);
        }
        else if (is<ExpressionList<T>>(expr)) {
            const auto& sub = get<ExpressionList<T>>(expr);
            T val = parseExpressionTree<T>(sub);
            values.push_back(val);
        }
    }
    while (!operators.empty()) {
        size_t largest = 0;
        size_t index = operators.size();
        for (size_t i = 0; i < operators.size(); i++) {
            auto priority = opPriority(operators[i]);
            if (priority > largest) {
                largest = priority;
                index = i;
            }
        }
        values[index] = getOpFunc<T>(operators[index])(values[index], values[index + 1]);
        values.erase(values.begin() + index + 1);
        operators.erase(operators.begin() + index);
    }
    return values[0];
}
constexpr std::array<Expression<int>, 3> _test0 = {1, '+', 2};
static_assert(parseExpressionTree<int>(_test0) == 3);


template <typename T>
constexpr std::pair<T, std::string_view> parseNumber(std::string_view expr) {
    T out;
    auto result = std::from_chars(expr.data(), expr.data() + expr.size(), out);
    if (result.ec == std::errc::invalid_argument ||
        result.ec == std::errc::result_out_of_range) {
        throw std::invalid_argument(
            std::format("Parse error: Invalid number: {}", expr));
    }
    return {out, expr.substr(0, result.ptr - expr.data())};
}
constexpr auto _test1 = parseNumber<int>("135="sv);
static_assert(_test1.first == 135);
static_assert(_test1.second == "135"sv);


template <typename T>
constexpr std::string_view
parseArgument(std::string_view expr, std::span<const std::string_view> args) {
    for (size_t i = 0; i < args.size(); i++) {
        if (expr.starts_with(args[i])) {
            return args[i];
        }
    }
    throw std::invalid_argument(
        std::format("Parse error: Unexpected argument: {}", expr));
}
constexpr auto _test2 = parseArgument<int>("a+b"sv, {{"a"sv, "b"sv}});
static_assert(_test2 == "a"sv);


template <typename T>
constexpr std::pair<ExpressionList<T>, std::string_view>
parseExpression(std::string_view expr, std::span<const std::string_view> args) {
    ExpressionList<T> expressions;
    std::string_view copy = expr;
    bool op = false;

    while (!expr.empty()) {
        if (isSpace(expr[0])) {
            auto numSpaces = std::min(expr.find_first_not_of(" \n"sv), expr.size());
            expr.remove_prefix(numSpaces);
            continue;
        }
    
        if (expr[0] == ')') {
            expr.remove_prefix(1);
            break;
        }

        if (op) {
            if (!isOp(expr[0])) {
                throw std::invalid_argument(std::format(
                    "Parse error: Expected an operator next: {}", expr));
            }
            expressions.push_back(expr[0]);
            expr.remove_prefix(1);
            op = false;
            continue;
        }

        if (expr[0] == '(') {
            expr.remove_prefix(1);
            auto result = parseExpression<T>(expr, args);
            expressions.push_back(std::move(result.first));
            expr.remove_prefix(result.second.size());
        } else if (isAlpha(expr[0])) {
            auto arg = parseArgument<T>(expr, args);
            expr.remove_prefix(arg.size());
            expressions.push_back(std::move(arg));
        } else if (isFloat(expr[0])) {
            auto result = parseNumber<T>(expr);
            expressions.push_back(std::move(result.first));
            expr.remove_prefix(result.second.size());
        } else {
            throw std::invalid_argument(
                std::format("Parse error: Unexpected token: {}", expr));
        }
        op = true;
    }
    copy.remove_suffix(expr.size());

    if (expressions.empty()) {
        throw std::invalid_argument(std::format("Parse error: Cannot have empty brackets: {}", copy));
    }
    if (!op) {
        throw std::invalid_argument(std::format(
            "Parse error: Must have an argument on either side of an operator: {}", copy));
    }

    return {expressions, copy};
}
constexpr auto expr = "   1   *   2   "sv;
constexpr auto _test31 = get<int>(parseExpression<int>(expr, {{"aa"sv, "bb"sv}}).first[0]);
static_assert(_test31 == 1);
constexpr auto _test32 = get<char>(parseExpression<int>(expr, {{"aa"sv, "bb"sv}}).first[1]);
static_assert(_test32 == '*');
constexpr auto _test33 = get<int>(parseExpression<int>(expr, {{"aa"sv, "bb"sv}}).first[2]);
static_assert(_test33 == 2);
constexpr auto _test34 = parseExpression<int>(expr, {{"aa"sv, "bb"sv}}).second;
static_assert(_test34 == expr);


template <size_t N>
constexpr std::array<std::string_view, N>
parseArguments(std::string_view args) {
    std::array<std::string_view, N> arguments{};
    size_t count = 0;
    size_t pos = 0;

    while (pos < args.size()) {
        // Skip over any whitespace.
        while (pos < args.size()&&  !isAlpha(args[pos])) {
            ++pos;
        }

        if (pos < args.size()&&  isAlpha(args[pos])) {
            // Get the length of the argument.
            size_t start = pos;
            while (pos < args.size()&&  isAlnum(args[pos])) {
                ++pos;
            }
            if (count < arguments.size()) {
                arguments[count] = args.substr(start, pos - start);
            }
            ++count;
        }
    }

    if (count != N) {
        throw std::invalid_argument(std::format(
            "Expected function to have {} argument{}", N, plural(N)));
    }

    return arguments;
}
constexpr auto args = " a , b "sv;
constexpr auto _test4 = parseArguments<2>(args);
static_assert(_test4.size() == 2);
static_assert(_test4[0] == "a"sv);
static_assert(_test4[1] == "b"sv);


constexpr auto parseFunction(std::string_view script) {
    auto open_paren_pos = script.find('(');
    auto close_paren_pos = script.find(')');
    if (open_paren_pos == std::string_view::npos ||
        close_paren_pos == std::string_view::npos ||
        close_paren_pos <= open_paren_pos) {
        throw std::invalid_argument(
            "Invalid script: brackets not found or in wrong order");
    }
    auto brackets =
        script.substr(open_paren_pos, close_paren_pos - open_paren_pos);

    auto open_brace_pos = script.find('{');
    auto close_brace_pos = script.find('}');
    if (open_brace_pos == std::string_view::npos ||
        close_brace_pos == std::string_view::npos ||
        close_brace_pos <= open_brace_pos) {
        throw std::invalid_argument(
            "Invalid script: braces not found or in wrong order");
    }
    auto braces =
        script.substr(open_brace_pos, close_brace_pos - open_brace_pos);

    auto args = brackets.substr(1, brackets.size() - 1);
    auto expr = braces.substr(1, braces.size() - 1);

    return std::make_pair(args, expr);
}
constexpr auto _test5 = parseFunction("(a,b){a+b}"sv);
static_assert(_test5.first == "a,b"sv);
static_assert(_test5.second == "a+b"sv);


template <typename T, typename... Args>
    requires are_all_same_v<T, Args...> && Number<T>
constexpr T parse(std::string_view script) {
    constexpr size_t N = sizeof...(Args);
    auto argsExpr = parseFunction(script);
    auto args = argsExpr.first;
    auto expr = argsExpr.second;
    auto arguments = parseArguments<N>(args);
    auto expressions = parseExpression<T>(expr, arguments).first;
    return parseExpressionTree<T>(expressions);
}
constexpr auto _test6 = parse<int>("(){3*5-4+(1-2)}"sv);
static_assert(_test6 == 10);

} // namespace script
