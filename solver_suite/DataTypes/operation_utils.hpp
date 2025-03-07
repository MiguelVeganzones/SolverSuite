#pragma once

#include "data_type_concepts.hpp"
#include <algorithm>
#include <cmath>
#include <concepts>
#include <ranges>
#include <type_traits>

namespace data_types::operation_utils
{

[[nodiscard]]
constexpr auto subscript(auto&& v, std::integral auto idx) noexcept
{
    if constexpr (std::ranges::range<std::remove_cvref_t<decltype(v)>>)
    {
        return *std::ranges::next(std::begin(v), idx);
    }
    else if constexpr (dt_concepts::ExpressionTemplate<std::remove_cvref_t<decltype(v)>>)
    {
        return v[idx];
    }
    else
    {
        return v;
    }
}

template <std::ranges::range T>
[[gnu::pure, nodiscard]]
auto distance(T const& p1, T const& p2) noexcept -> T
{
    return T(p2 - p1);
}

[[gnu::pure, nodiscard]]
auto l2_norm_sq(std::ranges::range auto const& v) noexcept ->
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
auto l2_norm(std::ranges::range auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    return std::sqrt(l2_norm_sq(v));
}

[[gnu::pure, nodiscard]]
auto linfinity_norm(std::ranges::range auto const& v) noexcept ->
    typename std::remove_cvref_t<decltype(v)>::value_type
{
    return std::ranges::max(v);
}

[[gnu::pure, nodiscard]]
auto normalize(std::ranges::range auto const& v) noexcept -> std::pair<
    std::remove_cvref_t<decltype(v)>,
    typename std::remove_cvref_t<decltype(v)>::value_type>
{
    const auto norm = l2_norm(std::forward<decltype(v)>(v));
    return { decltype(v)(v / norm), norm };
}

} // namespace data_types::operation_utils
