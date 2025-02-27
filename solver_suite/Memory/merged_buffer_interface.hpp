#pragma once

#include "buffer_concepts.hpp"
#include "buffer_config.hpp"
#include <iterator>
#include <utility>

namespace buffers::buffer_interface
{

template <
    typename T,
    mb_concepts::BufferShape              Interface_Shape,
    buffers::config::LayoutPolicy         Layout_Policy,
    std::size_t                           Padding_Stride,
    allocators::concepts::BufferAllocator Allocator>
struct physical_buffer
{
    using value_type                             = T;
    using index_t                                = std::size_t;
    using interface_shape_t                      = Interface_Shape;
    using underlying_shape_t                     = Interface_Shape;
    using size_type                              = typename shape_t::size_type;
    inline static constexpr auto s_layout_policy = Layout_Config;

    physical_buffer(shape_t shape) :
        shape_{ shape }
    {
    }

    constexpr physical_buffer(physical_buffer const&) noexcept = default;
    constexpr physical_buffer(physical_buffer&&) noexcept      = default;
    constexpr auto operator=(physical_buffer const&) noexcept
        -> physical_buffer&                                                  = default;
    constexpr auto operator=(physical_buffer&&) noexcept -> physical_buffer& = default;
    ~physical_buffer() noexcept                                              = default;

    [[nodiscard]]
    inline constexpr auto operator[](
        this auto&& self,
        size_type   idx_y,
        size_type   idx_x
    ) noexcept -> decltype(auto)
    {
        assert(idx_y < std::forward<decltype(self)>(self).interface_shape_.size_y());
        assert(idx_x < std::forward<decltype(self)>(self).interface_shape_.size_x());
        return std::forward<decltype(self)>(self)
            .data_[std::forward<decltype(self)>(self).translate_idx(idx_x, idx_y)];
    }

    [[nodiscard]]
    inline constexpr auto translate_idx(index_t idx_x, index_t idx_y) const noexcept
        -> index_t
    {
        if constexpr (s_layout_policy == config::LayoutPolicy::layout_column_major)
        {
            return idx_x * shape_.size_y() + idx_y;
        }
        else if constexpr (s_layout_policy == config::LayoutPolicy::layout_row_major)
        {
            return idx_x + shape_.size_x() * idx_y;
        }
        else
        {
            static_assert(false, "Layout policy is not in the list");
        }
    }

    interface_shape_t                            interface_shape_;
    underlying_shape_t                           underlying_shape_;
    std::array<value_type, shape_t::flat_size()> data_;
};

} // namespace buffers::buffer_interface
