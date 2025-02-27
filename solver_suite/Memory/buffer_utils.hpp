#pragma once

#include <cstddef>

namespace buffers::mb_utils
{

[[nodiscard, gnu::const]]
constexpr auto is_valid_alignment(std::size_t alignment) noexcept -> bool
{
    return alignment != 0 && ((alignment & (alignment - 1)) == 0);
}

} // namespace buffers::mb_utils
