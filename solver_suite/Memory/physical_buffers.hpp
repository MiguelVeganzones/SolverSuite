#pragma once

#include "buffer_concepts.hpp"
#include "buffer_config.hpp"
#include <iterator>
#include <utility>

namespace buffers::physical_buffer
{

template <
    typename T,
    mb_concepts::BufferShape      Shape,
    buffers::config::LayoutPolicy Layout_Config,
    allocators::concepts::BufferAllocator>
struct physical_buffer
{
    using value_type                             = T;
    using index_t                                = std::size_t;
    using shape_t                                = Shape;
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

    [[nodiscard]]
    inline constexpr auto begin(this auto&& self) noexcept -> decltype(auto)
    {
        return std::begin(std::forward<decltype(self)>(self).data_);
    }

    [[nodiscard]]
    inline constexpr auto end(this auto&& self) noexcept -> decltype(auto)
    {
        return std::end(std::forward<decltype(self)>(self).data_);
    }

    [[nodiscard]]
    inline constexpr auto cbegin() const -> container_t::const_iterator
    {
        return std::cbegin(data_);
    }

    [[nodiscard]]
    inline constexpr auto cend() const -> container_t::const_iterator
    {
        return std::cend(data_);
    }

    shape_t                                      shape_;
    std::array<value_type, shape_t::flat_size()> data_;
};

} // namespace buffers::physical_buffer
