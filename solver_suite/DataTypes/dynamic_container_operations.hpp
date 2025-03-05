#pragma once

#include "data_type_concepts.hpp"
#include "data_type_utils.hpp"
#include "operation_utils.hpp"
#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>

// Needs to be in the same namespace for name resolution of constructors,
// unfortunately
namespace data_types::dynamic_containers
{

template <typename Callable, typename... Operands>
    requires(dt_concepts::ValidExprOperand<Operands> && ...)
class expr
{
private:
    template <typename T>
    using storage_t =
        std::conditional_t<dt_concepts::DynamicArray<T>, T const&, std::decay_t<T>>;

public:
    inline static constexpr auto expression_template_disambiguator = 1;

    using callable_t      = Callable;
    using size_type       = std::size_t;
    using storage_tuple_t = std::tuple<storage_t<Operands>...>;

public:
    constexpr expr(size_type size, callable_t f, Operands const&... args)
        : m_args(args...)
        , m_f{ f }
        , m_size{ size }
    {
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> size_type
    {
        return m_size;
    }

    [[nodiscard]]
    inline constexpr auto operator[](std::integral auto idx) const
    {
        assert(idx < m_size);
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
    size_type       m_size;
};

template <std::size_t N, std::size_t I = 0>
    requires(N > I)
[[nodiscard]]
auto expr_reduce(auto const& range_a, auto const& range_b) -> auto
{
    assert(std::ranges::size(range_a) == N);
    assert(std::ranges::size(range_b) == N);

    const auto size = dt_utils::common_size(range_a[I], range_b[I]);
    auto       e    = expr{ size,
                   [](auto const& l, auto const& r) noexcept {
                       return std::invoke(std::multiplies{}, l, r);
                   },
                   range_a[I],
                   range_b[I] };
    if constexpr (I + 1 == N)
    {
        return e;
    }
    else
    {
        return expr{ size,
                     [](auto const& l, auto const& r) noexcept {
                         return std::invoke(std::plus{}, l, r);
                     },
                     e,
                     expr_reduce<N, I + 1>(range_a, range_b) };
    }
}

[[nodiscard]]
auto operator+(auto const& lhs, auto const& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto const& l, auto const& r) noexcept {
                     return std::invoke(std::plus{}, l, r);
                 },
                 lhs,
                 rhs };
}

[[nodiscard]]
auto operator-(auto const& lhs, auto const& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto const& l, auto const& r) noexcept {
                     return std::invoke(std::minus{}, l, r);
                 },
                 lhs,
                 rhs };
}

[[nodiscard]]
auto operator*(auto const& lhs, auto const& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto const& l, auto const& r) noexcept {
                     return std::invoke(std::multiplies{}, l, r);
                 },
                 lhs,
                 rhs };
}

[[nodiscard]]
auto operator/(auto const& lhs, auto const& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto const& l, auto const& r) noexcept {
                     return std::invoke(std::divides{}, l, r);
                 },
                 lhs,
                 rhs };
}

} // namespace data_types::dynamic_containers
