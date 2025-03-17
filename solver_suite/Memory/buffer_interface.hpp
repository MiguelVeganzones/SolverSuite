#pragma once

#include "buffer_config.hpp"
#include "error_handling.hpp"
#include <cassert>
#include <cstddef>
#include <span>

namespace buffers::buffer_interface
{

template <
    std::size_t                 Size_Y,
    std::size_t                 Size_X,
    buffer_config::LayoutPolicy Layout_Policy,
    std::size_t                 Layout_Minor_Stride>
class static_buffer_interface
{
public:
    // TODO: signed integer types may be faster due to a narrower contract
    using size_type                                  = std::size_t;
    using index_t                                    = std::size_t;
    using layout_policy_t                            = buffer_config::LayoutPolicy;
    static constexpr size_type s_size_y              = Size_Y;
    static constexpr size_type s_size_x              = Size_X;
    static constexpr auto      s_layout_policy       = Layout_Policy;
    static constexpr auto      s_layout_minor_stride = Layout_Minor_Stride;
    static constexpr size_type s_underlying_size_y   = [] constexpr {
        if constexpr (s_layout_policy == layout_policy_t::layout_row_major)
        {
            return Size_Y;
        }
        else if constexpr (s_layout_policy == layout_policy_t::layout_column_major)
        {
            return Size_X;
        }
        else
        {
            static_assert(false, "Layout policy not recognized or not implemented");
        }
    }();
    static constexpr size_type s_underlying_size_x = [] constexpr {
        if constexpr (s_layout_policy == layout_policy_t::layout_row_major)
        {
            static_assert(
                s_layout_minor_stride == 0 || Size_X <= s_layout_minor_stride,
                "User defined stride must be greater than or equal to the corresponfing "
                "extent"
            );
            return std::max(Size_X, s_layout_minor_stride);
        }
        else if constexpr (s_layout_policy == layout_policy_t::layout_column_major)
        {
            static_assert(
                s_layout_minor_stride == 0 || Size_Y <= s_layout_minor_stride,
                "User defined stride must be greater than or equal to the corresponfing "
                "extent"
            );
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
    static constexpr auto capacity_y() noexcept -> size_type
    {
        return size_y();
    }

    [[nodiscard, gnu::const]]
    static constexpr auto capacity_x() noexcept -> size_type
    {
        return size_x();
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size_y() noexcept -> size_type
    {
        return s_size_y;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto flat_size() noexcept -> size_type
    {
        return size_y() * size_x();
    }

    [[nodiscard, gnu::const]]
    static constexpr auto underlying_size_x() noexcept -> size_type
    {
        return s_underlying_size_x;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto underlying_size_y() noexcept -> size_type
    {
        return s_underlying_size_y;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto underlying_flat_size() noexcept -> size_type
    {
        return underlying_size_y() * underlying_size_x();
    }

    [[nodiscard, gnu::const]]
    constexpr auto flat_projection(index_t idx_y, index_t idx_x) const noexcept -> index_t
    {
        assert(idx_y < s_size_y);
        assert(idx_x < s_size_x);
        if constexpr (s_layout_policy == layout_policy_t::layout_column_major)
        {
            return idx_x * underlying_size_x() + idx_y;
        }
        else if constexpr (s_layout_policy == layout_policy_t::layout_row_major)
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
    using size_type                     = std::size_t;
    using index_t                       = std::size_t;
    using layout_policy_t               = buffer_config::LayoutPolicy;
    static constexpr size_type s_size_y = std::dynamic_extent;
    static constexpr size_type s_size_x = Size_X;
    static_assert(s_size_x > 0);

public:
    constexpr dynamic_length(
        size_type       length,
        size_type       capacity,
        layout_policy_t layout_policy,
        size_type       minor_stride
    ) noexcept
        : size_y_{ length }
        , capacity_y_{ std::max(capacity, size_y_) }
        , underlying_size_y_{ layout_policy == layout_policy_t::layout_row_major
                                  ? capacity_y_
                                  : s_size_x }
        , underlying_size_x_{ layout_policy == layout_policy_t::layout_row_major
                                  ? std::max(s_size_x, minor_stride)
                                  : std::max(capacity_y_, minor_stride) }
        , stride_y_{ layout_policy == layout_policy_t::layout_row_major
                         ? underlying_size_x_
                         : size_type{ 1 } }
        , stride_x_{ layout_policy == layout_policy_t::layout_row_major
                         ? size_type{ 1 }
                         : underlying_size_x_ }
        , layout_policy_{ layout_policy }
    {
        assert(
            layout_policy_ == layout_policy_t::layout_column_major ||
            layout_policy_ == layout_policy_t::layout_row_major
        );
        switch (layout_policy_)
        {
        case layout_policy_t::layout_column_major:
            assert(capacity_y_ <= minor_stride);
            break;
        case layout_policy_t::layout_row_major: assert(s_size_x <= minor_stride); break;
        default: utility::error_handling::assert_unreachable(); break;
        }
    }

    [[nodiscard]]
    constexpr auto capacity_y() const noexcept -> size_type
    {
        return capacity_y_;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto capacity_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard]]
    constexpr auto size_y() const noexcept -> size_type
    {
        return size_y_;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard]]
    constexpr auto flat_size() const noexcept -> size_type
    {
        return size_y() * size_x();
    }

    [[nodiscard]]
    constexpr auto underlying_size_x() const noexcept -> size_type
    {
        return underlying_size_x_;
    }

    [[nodiscard]]
    constexpr auto underlying_size_y() const noexcept -> size_type
    {
        return underlying_size_y_;
    }

    [[nodiscard]]
    constexpr auto underlying_flat_size() const noexcept -> size_type
    {
        return underlying_size_y() * underlying_size_x();
    }

    [[nodiscard, gnu::const]]
    constexpr auto flat_projection(index_t idx_y, index_t idx_x) const noexcept -> index_t
    {
        assert(idx_y < size_y_);
        assert(idx_x < s_size_x);
        return idx_y * stride_y_ + idx_x * stride_x_;
    }

private:
    size_type       size_y_;
    size_type       capacity_y_;
    size_type       underlying_size_y_;
    size_type       underlying_size_x_;
    size_type       stride_y_;
    size_type       stride_x_;
    layout_policy_t layout_policy_;
};

class dynamic_shape
{
public:
    using size_type                     = std::size_t;
    using index_t                       = std::size_t;
    using layout_policy_t               = buffer_config::LayoutPolicy;
    static constexpr size_type s_size_y = std::dynamic_extent;
    static constexpr size_type s_size_x = std::dynamic_extent;

public:
    constexpr dynamic_shape(
        size_type       size_y,
        size_type       capacity_y,
        size_type       size_x,
        size_type       capacity_x,
        layout_policy_t layout_policy,
        size_type       minor_stride
    ) noexcept
        : size_y_{ size_y }
        , size_x_{ size_x }
        , capacity_y_{ std::max(capacity_y, size_y_) }
        , capacity_x_{ std::max(capacity_x, size_x_) }
        , underlying_size_y_{ layout_policy == layout_policy_t::layout_row_major
                                  ? capacity_y_
                                  : capacity_x_ }
        , underlying_size_x_{ layout_policy == layout_policy_t::layout_row_major
                                  ? std::max(capacity_x_, minor_stride)
                                  : std::max(capacity_y_, minor_stride) }
        , stride_y_{ layout_policy == layout_policy_t::layout_row_major
                         ? underlying_size_x_
                         : size_type{ 1 } }
        , stride_x_{ layout_policy == layout_policy_t::layout_row_major
                         ? size_type{ 1 }
                         : underlying_size_x_ }
        , layout_policy_{ layout_policy }
    {
        assert(
            layout_policy_ == layout_policy_t::layout_column_major ||
            layout_policy_ == layout_policy_t::layout_row_major
        );
        switch (layout_policy_)
        {
        case layout_policy_t::layout_column_major:
            assert(capacity_y_ <= minor_stride);
            break;
        case layout_policy_t::layout_row_major:
            assert(capacity_x_ <= minor_stride);
            break;
        default: utility::error_handling::assert_unreachable(); break;
        }
    }

    [[nodiscard]]
    constexpr auto capacity_y() const noexcept -> size_type
    {
        return capacity_y_;
    }

    [[nodiscard]]
    constexpr auto capacity_x() const noexcept -> size_type
    {
        return capacity_x_;
    }

    [[nodiscard]]
    constexpr auto size_y() const noexcept -> size_type
    {
        return size_y_;
    }

    [[nodiscard]]
    constexpr auto size_x() const noexcept -> size_type
    {
        return size_x_;
    }

    [[nodiscard]]
    constexpr auto flat_size() const noexcept -> size_type
    {
        return size_y_ * size_x_;
    }

    [[nodiscard]]
    constexpr auto underlying_size_x() const noexcept -> size_type
    {
        return underlying_size_x_;
    }

    [[nodiscard]]
    constexpr auto underlying_size_y() const noexcept -> size_type
    {
        return underlying_size_y_;
    }

    [[nodiscard]]
    constexpr auto underlying_flat_size() const noexcept -> size_type
    {
        return underlying_size_x_ * underlying_size_y_;
    }

    [[nodiscard, gnu::const]]
    constexpr auto flat_projection(index_t idx_y, index_t idx_x) const noexcept -> index_t
    {
        assert(idx_y < size_y_);
        assert(idx_x < size_x_);
        return idx_y * stride_y_ + idx_x * stride_x_;
    }

private:
    // TODO: Capacity is redundant since it can be obtained at runtime knowing
    // the layout and the underlying size. Look into it, most likely doesnt make sense to
    // trade space for efficiency in an object like this but they are definitely
    // unnecessarily large
    size_type       size_y_;
    size_type       size_x_;
    size_type       capacity_y_;
    size_type       capacity_x_;
    size_type       underlying_size_y_;
    size_type       underlying_size_x_;
    size_type       stride_y_;
    size_type       stride_x_;
    layout_policy_t layout_policy_;
};

} // namespace buffers::buffer_interface
