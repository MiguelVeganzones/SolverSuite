#pragma once

#include "concepts.hpp"
#include "error_handling.hpp"
#include "memory_buffer_concepts.hpp"
#include "memory_buffer_utils.hpp"
#include <array>
#include <cassert>
#include <limits>
#include <span>

namespace memory_buffers
{

template <std::size_t Size, std::size_t Dim>
struct static_data_shape
{
    using size_type                          = std::size_t;
    inline static constexpr size_type s_size = Size;
    inline static constexpr size_type s_dim  = Dim;

    static_assert(s_size > 0);
    static_assert(s_dim > 0);

    [[nodiscard, gnu::const]]
    inline static constexpr auto capacity() noexcept -> size_type
    {
        return s_size;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto size() noexcept -> size_type
    {
        return s_size;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto dim() noexcept -> size_type
    {
        return s_dim;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto flattened_size() noexcept -> size_type
    {
        return s_size * s_dim;
    }
};

template <std::size_t Dim>
struct dynamic_data_shape
{
    using size_type                          = std::size_t;
    inline static constexpr size_type s_size = std::dynamic_extent;
    inline static constexpr size_type s_dim  = Dim;

    static_assert(s_dim > 0);

    size_type size_;
    size_type capacity_; // TODO: Should this be const?

    [[nodiscard]]
    inline constexpr auto capacity() const noexcept -> size_type
    {
        return capacity_;
    }

    [[nodiscard]]
    inline constexpr auto size() const noexcept -> size_type
    {
        return size_;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto dim() noexcept -> size_type
    {
        return s_dim;
    }

    [[nodiscard]]
    inline constexpr auto flattened_size() const noexcept -> size_type
    {
        return size_ * s_dim;
    }
};

template <buffers::mb_concepts::VectorBufferShape Shape_Type>
struct physical_buffer_manager
{
    using shape_t                       = Shape_Type;
    using size_type                     = typename shape_t::size_type;
    inline static constexpr auto s_size = shape_t::s_size;
    inline static constexpr auto s_dim  = shape_t::s_dim;

    physical_buffer_manager(Shape_Type data_shape) :
        data_shape_(data_shape)
    {
    }

    shape_t data_shape_;

    [[nodiscard]]
    inline constexpr auto capacity() const noexcept -> size_type
    {
        return data_shape_.capacity();
    }

    [[nodiscard]]
    inline constexpr auto size() const noexcept -> size_type
    {
        return data_shape_.size();
    }

    [[nodiscard]]
    inline constexpr auto dim() const noexcept -> size_type
    {
        return data_shape_.size();
    }

    [[nodiscard]]
    inline constexpr auto flattened_size() const noexcept -> size_type
    {
        return data_shape_.flattened_size();
    }
};

} // namespace memory_buffers
