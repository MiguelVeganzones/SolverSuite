#pragma once

#include "data_type_concepts.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <ranges>
#include <type_traits>

namespace data_types::operation_utils
{

template <std::size_t N, std::size_t I = 0>
    requires(N > I)
[[nodiscard]]
constexpr auto expr_reduce(
    std::ranges::input_range auto const& range_a,
    std::ranges::input_range auto const& range_b
) noexcept -> decltype(auto)
{
    assert(std::ranges::size(range_a) == N);
    assert(std::ranges::size(range_b) == N);

    const auto e = range_a[I] * range_b[I];
    if constexpr (I + 1 == N)
    {
        return e;
    }
    else
    {
        return e + expr_reduce<N, I + 1>(range_a, range_b);
    }
}

template <typename T>
[[nodiscard]]
constexpr auto subscript(T&& v, std::integral auto const idx) noexcept
{
    if constexpr (dt_concepts::Indexable<T>)
    {
        return v[idx];
    }
    else if constexpr (std::ranges::range<T>)
    {
        return *std::ranges::next(std::begin(v), idx);
    }
    else
    {
        return v;
    }
}

template <std::ranges::range T>
[[gnu::pure, nodiscard]]
constexpr auto distance(T const& p1, T const& p2) noexcept -> T
{
    return T(p2 - p1);
}

[[gnu::pure, nodiscard]]
constexpr auto l2_norm_sq(std::ranges::range auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    using value_type = typename std::remove_cvref_t<decltype(v)>::value_type;
    const auto n     = v.size();
    value_type ret{};
    for (auto i = decltype(n){}; i != n; ++i)
    {
        ret += v[i] * v[i];
    }
    return ret;
}

[[gnu::pure, nodiscard]]
constexpr auto l2_norm_sq(std::ranges::range auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
    requires dt_concepts::StaticArray<std::remove_reference_t<decltype(v)>>
{
    using value_type = typename std::remove_cvref_t<decltype(v)>::value_type;
    constexpr auto n = std::remove_reference_t<decltype(v)>::size();
    value_type     ret{};
    for (auto i = decltype(n){}; i != n; ++i)
    {
        ret += v[i] * v[i];
    }
    return ret;
}

[[gnu::pure, nodiscard]]
constexpr auto l2_norm(std::ranges::range auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    return std::sqrt(l2_norm_sq(v));
}

[[gnu::pure, nodiscard]]
constexpr auto linfinity_norm(std::ranges::range auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    return std::ranges::max(v | std::views::transform([](auto const& e) {
                                return std::abs(e);
                            }));
}

[[gnu::pure, nodiscard]]
constexpr auto normalize(std::ranges::range auto& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    const auto norm = l2_norm(std::forward<decltype(v)>(v));
    std::ranges::transform(v, [norm](auto& e) { return e / norm; });
    return norm;
}

} // namespace data_types::operation_utils
