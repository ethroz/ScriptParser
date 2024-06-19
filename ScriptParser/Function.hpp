#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename> class function;

template <typename Ret, typename... Args>
class function<Ret(Args...)> {
public:
    constexpr function() noexcept = default;

    using FuncType = Ret (*)(Args...);

    template <typename T>
    constexpr function(T&& f) noexcept
        requires std::is_convertible_v<T, FuncType>
        : func(std::move(f)) {}

    constexpr Ret operator()(Args... args) const {
        if (func == nullptr) {
            throw std::runtime_error("Function pointer is null");
        }
        return func(std::forward<Args>(args)...);
    }

    constexpr operator bool() const noexcept { return func != nullptr; }

private:
    FuncType func = nullptr;
};
