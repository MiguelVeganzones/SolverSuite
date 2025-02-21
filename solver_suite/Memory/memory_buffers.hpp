#pragma once

#include "concepts.hpp"
#include "memory_buffer_utils.hpp"
#include <array>
#include <span>

namespace memory_buffers
{

template <std::size_t Size, std::size_t Dim>
struct data_shape;

template <std::size_t Size, std::size_t Dim>
struct data_shape
{
    using size_type                          = std::size_t;
    inline static constexpr size_type s_size = Size;
    inline static constexpr size_type s_dim  = Dim;

    [[nodiscard, gnu::const]]
    inline static constexpr auto flattened_size() noexcept -> size_type
    {
        if constexpr (s_dim == size_type{ 1 })
        {
            return s_size;
        }
        else
        {
            return s_size * s_dim;
        }
    }
};

template <std::size_t Dim>
struct data_shape<std::dynamic_extent, Dim>
{
    using size_type                          = std::size_t;
    inline static constexpr size_type s_size = std::dynamic_extent;
    inline static constexpr size_type s_dim  = Dim;

    size_type dynamic_size;

    [[nodiscard]]
    inline constexpr auto flattened_size() noexcept -> size_type
    {
        if constexpr (s_dim == size_type{ 1 })
        {
            return dynamic_size;
        }
        else
        {
            return dynamic_size * s_dim;
        }
    }
};

enum struct LayoutPolicy
{
    layout_dim_major,
    layout_sample_major,
    layout_sample_major_padded,
};

template <LayoutPolicy Layout_Policy, std::size_t Major_Alignment>
struct layout_info;

template <LayoutPolicy Layout_Policy, std::size_t = 0>
struct layout_info
{
    inline static constexpr auto s_layout_policy = Layout_Policy;
};

template <std::size_t Major_Alignment>
struct layout_info<LayoutPolicy::layout_sample_major_padded, Major_Alignment>
{
    static_assert(mb_utils::is_valid_alignment(Major_Alignment));
    inline static constexpr auto s_layout_policy =
        LayoutPolicy::layout_sample_major_padded;
    inline static constexpr auto s_major_alignment = Major_Alignment;
};

template <data_shape Data_Shape, layout_info Layout_Info>
struct buffer_size_manager
{
    inline static constexpr auto s_data_shape    = Data_Shape;
    inline static constexpr auto s_size          = s_data_shape.s_size;
    inline static constexpr auto s_dim           = s_data_shape.s_dim;
    inline static constexpr auto s_layout_info   = Layout_Info;
    inline static constexpr auto s_layout_policy = s_layout_info.s_layout_policy;
    using size_type = typename decltype(s_data_shape)::size_type;
    using index_t   = size_type;

    inline static constexpr auto s_buffer_size_info = []() constexpr {
        // Return data_row_size, buffer_row_size, row_count
        if constexpr (s_layout_policy == LayoutPolicy::layout_dim_major)
        {
            return std::array{ s_data_shape.s_size,
                               s_data_shape.s_size,
                               s_data_shape.s_dim };
        }
        else if constexpr (s_layout_policy == LayoutPolicy::layout_sample_major)
        {
            return std::array{ s_data_shape.s_dim,
                               s_data_shape.s_dim,
                               s_data_shape.s_size };
        }
        else if constexpr (s_layout_policy == LayoutPolicy::layout_sample_major_padded)
        {
            return std::array{
                s_data_shape.s_dim,
                std::max(s_data_shape.s_dim, s_layout_info.s_major_alignment),
                s_data_shape.s_size
            };
        }
    }();

    inline static constexpr auto s_row_size        = s_buffer_size_info[0];
    inline static constexpr auto s_padded_row_size = s_buffer_size_info[1];
    inline static constexpr auto s_row_count       = s_buffer_size_info[2];

    [[nodiscard, gnu::const]]
    inline static constexpr auto full_flattened_size() noexcept -> size_type
    {
        if constexpr (s_row_size == size_type{ 1 })
        {
            return s_row_count;
        }
        else
        {
            return s_row_count * s_padded_row_size;
        }
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto translate_idx(
        index_t sample_idx,
        index_t dim_idx
    ) noexcept -> index_t
        requires(s_size != std::dynamic_extent && s_dim != 1)
    {
        if constexpr (s_layout_policy == LayoutPolicy::layout_dim_major)
        {
            return s_row_size * dim_idx + sample_idx;
        }
        else if constexpr (s_layout_policy == LayoutPolicy::layout_sample_major)
        {
            return s_row_size * sample_idx + dim_idx;
        }
        else if constexpr (s_layout_policy == LayoutPolicy::layout_sample_major_padded)
        {
            return s_padded_row_size * sample_idx + dim_idx;
        }
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto translate_idx(index_t sample_idx) noexcept -> index_t
        requires(s_size != std::dynamic_extent && s_dim == 1)
    {
        return sample_idx;
    }
};

template <utility::concepts::arithmetic T, data_shape Data_Shape, layout_info Layout_Info>
    requires(!mb_utils::is_dynamic_extent_v<Data_Shape>)
struct static_stage_buffer
{
    using value_type                           = T;
    inline static constexpr auto s_data_shape  = Data_Shape;
    inline static constexpr auto s_layout_info = Layout_Info;
    inline static constexpr auto s_buffer_size_manager =
        buffer_size_manager<s_data_shape, s_layout_info>{};

    std::array<T, s_buffer_size_manager.full_flattened_size()> data_;

    [[nodiscard]]
    inline constexpr auto operator[](
        this auto&& self,
        std::size_t sample_idx,
        std::size_t dim_idx
    ) noexcept -> auto&&
    {
        // TODO: Check bounds
        // std::forward<decltype(self)>(self).assert_in_bounds(idx);
        return std::forward<decltype(self)>(self)
            .data_[s_buffer_size_manager.translate_idx(sample_idx, dim_idx)];
    }
};


} // namespace memory_buffers
