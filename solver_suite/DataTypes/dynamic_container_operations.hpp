#pragma once

#include "operation_utils.hpp"
#include <functional>
#include <tuple>

// Needs to be in the same namespace for name resolution of constructors,
// unfortunately
namespace data_types::dynamic_containers
{

template <typename T>
concept expression_template = requires(T) { T::expression_template_disambiguator; };

template <typename Callable, typename... Operands>
class expr
{
public:
    inline static constexpr auto expression_template_disambiguator = 1;
    using callable_t                                               = Callable;

public:
    constexpr expr(callable_t f, Operands const&... args)
        : m_args(args...)
        , m_f{ f }
    {
    }

    [[nodiscard]]
    inline constexpr auto operator[](std::integral auto idx) const
    {
        const auto apply_at_index =
            [this, idx] [[nodiscard]]
            (Operands const&... args) constexpr noexcept {
                return m_f(operation_utils::subscript(args, idx)...);
            };
        return std::apply(apply_at_index, m_args);
    }

private:
    std::tuple<Operands const&...> m_args;
    callable_t                     m_f;
};

auto operator+(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
                    return std::invoke(
                        std::plus{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator-(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
                    return std::invoke(
                        std::minus{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator*(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
                    return std::invoke(
                        std::multiplies{},
                        std::forward<decltype(l)>(l),
                        std::forward<decltype(r)>(r)
                    );
                },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator/(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    return expr{ [](auto&& l, auto&& r) {
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
