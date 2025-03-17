#pragma once

#include <cstddef>

namespace buffers::buffer_config
{

enum struct LayoutPolicy
{
    layout_column_major,
    layout_row_major,
};

template <std::size_t Padding_Stride>
struct layout_stride;

template <>
struct layout_stride<0uz>
{
    using size_type                     = std::size_t;
    static constexpr auto      s_padded = false;
    static constexpr size_type value    = static_cast<size_type>(0);
};

template <std::size_t Padding_Stride>
struct layout_stride
{
    using size_type                     = std::size_t;
    static constexpr auto      s_padded = true;
    static constexpr size_type value    = static_cast<size_type>(Padding_Stride);
};

} // namespace buffers::buffer_config
