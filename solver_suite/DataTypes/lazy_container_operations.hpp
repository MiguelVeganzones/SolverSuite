#pragma once

#include "data_type_concepts.hpp"
#include "data_type_utils.hpp"
#include "operation_utils.hpp"
#include <concepts>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>

// Needs to be in the same namespace for name resolution of constructors,
// unfortunately
namespace data_types::dynamic_containers
{

template <typename Callable, typename... Operands>
    requires(dt_concepts::ValidExprOperand<Operands> && ...)
class expr : dt_concepts::expression_templates_base
{
private:
    template <typename T>
    using storage_t =
        std::conditional_t<dt_concepts::DynamicArray<T>, T const&, std::decay_t<T>>;

public:
    using callable_t      = Callable;
    using size_type       = std::size_t;
    using storage_tuple_t = std::tuple<storage_t<Operands>...>;

public:
    constexpr expr(callable_t f, Operands const&... args)
        : m_args(args...)
        , m_f{ f }
    {
    }

    [[nodiscard]]
    inline constexpr auto operator[](std::integral auto idx) const
    {
        return std::apply(
            [this, idx](auto&&... args) noexcept {
                return m_f(
                    operation_utils::subscript(std::forward<decltype(args)>(args), idx)...
                );
            },
            m_args
        );
    }

private:
    storage_tuple_t m_args;
    callable_t      m_f;
};

[[nodiscard]]
auto operator+(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) noexcept {
                    return std::invoke(
                        std::plus{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

[[nodiscard]]
auto operator-(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) noexcept {
                    return std::invoke(
                        std::minus{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

[[nodiscard]]
auto operator*(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) noexcept {
                    return std::invoke(
                        std::multiplies{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

[[nodiscard]]
auto operator/(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) noexcept {
                    return std::invoke(
                        std::divides{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

} // namespace data_types::dynamic_containers
