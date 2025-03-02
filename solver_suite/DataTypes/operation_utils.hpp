#pragma once

#include <concepts>
#include <ranges>

namespace data_types::operation_utils
{

template <typename T>
[[nodiscard]]
constexpr auto subscript(T const& v, std::integral auto idx) noexcept
{
    if constexpr (std::ranges::range<T>)
    {
        return *std::ranges::next(std::begin(v), idx);
    }
    else
    {
        return v;
    }
}

} // namespace data_types::operation_utils
