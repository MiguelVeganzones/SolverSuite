#pragma once

#include <cassert>
#include <cstddef>

namespace data_types::buffer_config
{

enum struct LayoutPolicy
{
    layout_column_major,
    layout_row_major,
};

struct layout_stride
{
    using size_type = std::size_t;

    constexpr layout_stride(size_type stride)
        : value_{ stride }
    {
    }

    [[nodiscard]]
    constexpr auto value() const noexcept -> size_type
    {
        return value_;
    }

    [[nodiscard]]
    constexpr auto padded() const noexcept -> bool
    {
        return value_ != size_type{};
    }

    size_type value_;
};

} // namespace data_types::buffer_config
