#pragma once

#include "data_type_concepts.hpp"
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

} // namespace data_types::operation_utils
