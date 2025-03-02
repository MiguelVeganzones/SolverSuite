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
struct layout_padding;

template <>
struct layout_padding<0uz>
{
    static constexpr auto s_padded = false;
    static constexpr auto value    = 0;
};

template <std::size_t Padding_Stride>
struct layout_padding
{
    static constexpr auto s_padded = true;
    static constexpr auto value    = Padding_Stride;
};

} // namespace buffers::buffer_config
