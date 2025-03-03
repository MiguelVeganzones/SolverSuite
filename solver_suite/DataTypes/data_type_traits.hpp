#pragma once

#include <type_traits>

namespace data_types::dt_traits
{

template <typename T1, typename T2>
struct is_same_size : std::false_type
{
};

template <typename T1, typename T2>
    requires(T1::size == T2::size)
struct is_same_size<T1, T2> : std::true_type
{
};

template <typename T1, typename T2>
constexpr bool is_same_size_v = is_same_size<T1, T2>::value;

} // namespace data_types::dt_traits
