#pragma once

#include "data_type_concepts.hpp"
#include <concepts.hpp>
#include <utility>

namespace data_types::dt_utils
{

[[nodiscard]]
constexpr auto common_size(
    dt_concepts::SizedInstance auto const& a,
    dt_concepts::SizedInstance auto const& b
) noexcept -> auto
{
    assert(a.size() == b.size());
    // Returning the max size is safer because it will lead to out of index
    // access
    return std::max(a.size(), b.size());
}

[[nodiscard]]
constexpr auto common_size(
    dt_concepts::SizedInstance auto const& a,
    auto const&                            b
) noexcept -> decltype(std::declval<decltype(a)>().size())
{
    return a.size();
}

[[nodiscard]]
constexpr auto common_size(
    auto const&                            a,
    dt_concepts::SizedInstance auto const& b
) noexcept -> decltype(std::declval<decltype(b)>().size())
{
    return b.size();
}

} // namespace data_types::dt_utils
