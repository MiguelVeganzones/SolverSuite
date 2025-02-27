#pragma once

#include "buffer_concepts.hpp"
#include "buffer_config.hpp"
#include <cstddef>
#include <span>
#include <type_traits>

namespace buffers::buffer_size
{

template <std::size_t Size_Y, std::size_t Size_X>
struct static_shape
{
    using size_type                            = std::size_t;
    inline static constexpr size_type s_size_y = Size_Y;
    inline static constexpr size_type s_size_x = Size_X;

    static_assert(s_size_x > 0);
    static_assert(s_size_y > 0);

    [[nodiscard, gnu::const]]
    inline static constexpr auto capacity_y() noexcept -> size_type
    {
        return s_size_y;
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto capacity_x() noexcept -> size_type
    {
        return s_size_x;
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
        return s_size_x * s_size_y;
    }
};

template <std::size_t Size_X>
struct dynamic_length
{
    using size_type                            = std::size_t;
    inline static constexpr size_type s_size_y = std::dynamic_extent;
    inline static constexpr size_type s_size_x = Size_X;

    static_assert(s_size_x > 0);

    size_type size_y_;
    size_type capacity_y_;

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
};

struct dynamic_shape
{
    using size_type                            = std::size_t;
    inline static constexpr size_type s_size_y = std::dynamic_extent;
    inline static constexpr size_type s_size_x = std::dynamic_extent;

    size_type size_y_;
    size_type size_x_;
    size_type capacity_y_;
    size_type capacity_x_;

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
};

} // namespace buffers::buffer_size
