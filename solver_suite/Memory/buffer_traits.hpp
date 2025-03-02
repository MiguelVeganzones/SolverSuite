#pragma once

#include <span>
#include <type_traits>

namespace buffers::buffer_traits
{

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
constexpr bool is_dynamic_extent_v = is_dynamic_extent<T>::value;


} // namespace buffers::buffer_traits
