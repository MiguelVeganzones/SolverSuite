#pragma once

#include <cstddef>

namespace buffers::config
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
    inline static constexpr auto s_padded = false;
    inline static constexpr auto value    = 0;
};

template <std::size_t Padding_Stride>
struct layout_padding
{
    inline static constexpr auto s_padded = true;
    inline static constexpr auto value    = Padding_Stride;
};

} // namespace buffers::config
