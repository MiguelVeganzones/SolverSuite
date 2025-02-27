#pragma once

#include "buffer_config.hpp"
#include "error_handling.hpp"
#include <cassert>
#include <cstddef>
#include <span>

namespace buffers::buffer_interface
{

template <
    std::size_t                   Size_Y,
    std::size_t                   Size_X,
    buffers::config::LayoutPolicy Layout_Policy,
    std::size_t                   Layout_Minor_Stride>
class static_shape
{
public:
    using size_type                                         = std::size_t;
    using index_t                                           = std::size_t;
    inline static constexpr size_type s_size_y              = Size_Y;
    inline static constexpr size_type s_size_x              = Size_X;
    inline static constexpr auto      s_layout_policy       = Layout_Policy;
    inline static constexpr auto      s_layout_minor_stride = Layout_Minor_Stride;
    inline static constexpr size_type s_underlying_size_y   = [] constexpr {
        if constexpr (s_layout_policy == config::LayoutPolicy::layout_row_major)
        {
            return Size_Y;
        }
        else if constexpr (s_layout_policy == config::LayoutPolicy::layout_column_major)
        {
            return Size_X;
        }
        else
        {
            static_assert(false, "Layout policy not recognized or not implemented");
        }
    }();
    inline static constexpr size_type s_underlying_size_x = [] constexpr {
        if constexpr (s_layout_policy == config::LayoutPolicy::layout_row_major)
        {
            return std::max(Size_X, s_layout_minor_stride);
        }
        else if constexpr (s_layout_policy == config::LayoutPolicy::layout_column_major)
        {
            return std::max(Size_Y, s_layout_minor_stride);
        }
        else
        {
            static_assert(false, "Layout policy not recognized or not implemented");
        }
    }();

    static_assert(s_size_x > 0);
    static_assert(s_size_y > 0);

    [[nodiscard, gnu::const]]
    inline static constexpr auto capacity_y() noexcept -> size_type
    {
        return size_y();
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto capacity_x() noexcept -> size_type
    {
        return size_x();
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto size_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto size_y() noexcept -> size_type
    {
        return s_size_y;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto flat_size() noexcept -> size_type
    {
        return size_y() * size_x();
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto underlying_size_x() noexcept -> size_type
    {
        return s_underlying_size_x;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto underlying_size_y() noexcept -> size_type
    {
        return s_underlying_size_y;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto underlying_flat_size() noexcept -> size_type
    {
        return underlying_size_y() * underlying_size_x();
    }

    [[nodiscard, gnu::const]]
    inline constexpr auto translate_idx(index_t idx_y, index_t idx_x) const noexcept
        -> index_t
    {
        if constexpr (s_layout_policy == config::LayoutPolicy::layout_column_major)
        {
            return idx_x * underlying_size_x() + idx_y;
        }
        else if constexpr (s_layout_policy == config::LayoutPolicy::layout_row_major)
        {
            return idx_x + underlying_size_x() * idx_y;
        }
        else
        {
            static_assert(false, "Layout policy not recognized or not implemented");
        }
    }
};

template <std::size_t Size_X>
class dynamic_length
{
public:
    using size_type                            = std::size_t;
    using index_t                              = std::size_t;
    inline static constexpr size_type s_size_y = std::dynamic_extent;
    inline static constexpr size_type s_size_x = Size_X;
    static_assert(s_size_x > 0);

public:
    dynamic_length(
        size_type            length,
        size_type            capacity,
        config::LayoutPolicy layout_policy,
        size_type            minor_stride
    ) :
        size_y_{ length },
        capacity_y_{ std::max(capacity, size_y_) },
        underlying_size_y_{ layout_policy == config::LayoutPolicy::layout_row_major
                                ? capacity_y_
                                : s_size_x },
        underlying_size_x_{ layout_policy == config::LayoutPolicy::layout_row_major
                                ? std::max(s_size_x, minor_stride)
                                : std::max(capacity_y_, minor_stride) },
        stride_y_{ layout_policy == config::LayoutPolicy::layout_row_major
                       ? underlying_size_x_
                       : size_type{ 1 } },
        stride_x_{ layout_policy == config::LayoutPolicy::layout_row_major
                       ? size_type{ 1 }
                       : underlying_size_x_ },
        layout_policy_{ layout_policy }
    {
        assert(
            layout_policy_ == config::LayoutPolicy::layout_column_major ||
            layout_policy_ == config::LayoutPolicy::layout_row_major
        );
    }

    [[nodiscard]]
    inline constexpr auto capacity_y() const noexcept -> size_type
    {
        return capacity_y_;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto capacity_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard]]
    inline constexpr auto size_y() const noexcept -> size_type
    {
        return size_y_;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto size_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard]]
    inline constexpr auto flat_size() const noexcept -> size_type
    {
        return size_y_ * s_size_x;
    }

    [[nodiscard]]
    inline constexpr auto underlying_size_x() const noexcept -> size_type
    {
        return underlying_size_x_;
    }

    [[nodiscard]]
    inline constexpr auto underlying_size_y() const noexcept -> size_type
    {
        return underlying_size_y_;
    }

    [[nodiscard]]
    inline constexpr auto underlying_flat_size() const noexcept -> size_type
    {
        return underlying_size_x_ * underlying_size_y_;
    }

    [[nodiscard, gnu::const]]
    inline constexpr auto translate_idx(index_t idx_y, index_t idx_x) const noexcept
        -> index_t
    {
        return idx_y * stride_y_ + idx_x * stride_x_;
    }

private:
    size_type            size_y_;
    size_type            capacity_y_;
    size_type            underlying_size_y_;
    size_type            underlying_size_x_;
    size_type            stride_y_;
    size_type            stride_x_;
    config::LayoutPolicy layout_policy_;
};

class dynamic_shape
{
public:
    using size_type                            = std::size_t;
    using index_t                              = std::size_t;
    inline static constexpr size_type s_size_y = std::dynamic_extent;
    inline static constexpr size_type s_size_x = std::dynamic_extent;

public:
    dynamic_shape(
        size_type            size_y,
        size_type            capacity_y,
        size_type            size_x,
        size_type            capacity_x,
        config::LayoutPolicy layout_policy,
        size_type            minor_stride
    ) :
        size_y_{ size_y },
        size_x_{ size_x },
        capacity_y_{ std::max(capacity_y, size_y_) },
        capacity_x_{ std::max(capacity_x, size_x_) },
        underlying_size_y_{ layout_policy == config::LayoutPolicy::layout_row_major
                                ? capacity_y_
                                : capacity_x_ },
        underlying_size_x_{ layout_policy == config::LayoutPolicy::layout_row_major
                                ? std::max(capacity_x_, minor_stride)
                                : std::max(capacity_y_, minor_stride) },
        stride_y_{ layout_policy == config::LayoutPolicy::layout_row_major
                       ? underlying_size_x_
                       : size_type{ 1 } },
        stride_x_{ layout_policy == config::LayoutPolicy::layout_row_major
                       ? size_type{ 1 }
                       : underlying_size_x_ },
        layout_policy_{ layout_policy }
    {
        assert(
            layout_policy_ == config::LayoutPolicy::layout_column_major ||
            layout_policy_ == config::LayoutPolicy::layout_row_major
        );
    }

    [[nodiscard]]
    inline constexpr auto capacity_y() const noexcept -> size_type
    {
        return capacity_y_;
    }

    [[nodiscard]]
    inline constexpr auto capacity_x() const noexcept -> size_type
    {
        return capacity_x_;
    }

    [[nodiscard]]
    inline constexpr auto size_y() const noexcept -> size_type
    {
        return size_y_;
    }

    [[nodiscard]]
    inline constexpr auto size_x() const noexcept -> size_type
    {
        return size_x_;
    }

    [[nodiscard]]
    inline constexpr auto flat_size() const noexcept -> size_type
    {
        return size_y_ * size_x_;
    }

    [[nodiscard]]
    inline constexpr auto underlying_size_x() const noexcept -> size_type
    {
        return underlying_size_x_;
    }

    [[nodiscard]]
    inline constexpr auto underlying_size_y() const noexcept -> size_type
    {
        return underlying_size_y_;
    }

    [[nodiscard]]
    inline constexpr auto underlying_flat_size() const noexcept -> size_type
    {
        return underlying_size_x_ * underlying_size_y_;
    }

    [[nodiscard, gnu::const]]
    inline constexpr auto translate_idx(index_t idx_y, index_t idx_x) const noexcept
        -> index_t
    {
        return idx_y * stride_y_ + idx_x * stride_x_;
    }

private:
    // TODO: Capacity is redundant, since it can be obtained at runtime knowing
    // the layout and the underlying size. Look into it, maybe it makes sense to
    // trade space for efficiency, this would very much be application specific
    size_type            size_y_;
    size_type            size_x_;
    size_type            capacity_y_;
    size_type            capacity_x_;
    size_type            underlying_size_y_;
    size_type            underlying_size_x_;
    size_type            stride_y_;
    size_type            stride_x_;
    config::LayoutPolicy layout_policy_;
};

} // namespace buffers::buffer_interface
