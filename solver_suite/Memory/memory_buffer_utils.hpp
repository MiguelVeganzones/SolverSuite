#pragma once

#include <span>

namespace memory_buffers::mb_utils
{

[[nodiscard, gnu::const]]
constexpr auto is_valid_alignment(std::size_t alignment) noexcept -> bool
{
    return alignment != 0 && ((alignment & (alignment - 1)) == 0);
}

template <auto T>
struct is_dynamic_extent : std::false_type
{
};

template <auto T>
    requires(T.s_Size == std::dynamic_extent)
struct is_dynamic_extent<T> : std::true_type
{
};

template <auto T>
inline constexpr bool is_dynamic_extent_v = is_dynamic_extent<T>::value;

} // namespace memory_buffers::mb_utils
