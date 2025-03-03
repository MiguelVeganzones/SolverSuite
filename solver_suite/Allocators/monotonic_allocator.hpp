#pragma once

#include <cassert>
#include <cstddef>
#include <exception>
#include <type_traits>

#ifndef ALLOCATOR_DEBUG_NAN_INITIALIZE
#ifndef NDEBUG
#define ALLOCATOR_DEBUG_INITIALIZE 1
#if ALLOCATOR_DEBUG_INITIALIZE
#include <algorithm>
#include <limits>
#define ALLOCATOR_DEBUG_INITIALIZE_VALUE std::numeric_limits<value_type>::quiet_NaN()
#define ALLOCATOR_DEBUG_DEFAULT_ALLOCATE_VALUE                                           \
    std::numeric_limits<value_type>::quiet_NaN()
#endif
#endif
#endif

namespace allocators
{

template <typename T, std::size_t N>
    requires std::is_trivially_destructible_v<T>
class static_monotonic_allocator
{
public:
    using size_type            = std::size_t;
    using value_type           = std::remove_cvref_t<T>;
    using pointer              = value_type*;
    using const_pointer        = value_type const*;
    using reference            = value_type&;
    using const_reference      = value_type const&;
    using difference_type      = std::ptrdiff_t;
    static constexpr auto size = N;

    template <typename U>
    struct rebind
    {
        using other = static_monotonic_allocator<U, size>;
    };

    constexpr auto reset() noexcept -> void
    {
        cursor_ = size_type{};
    }

    [[nodiscard]]
    constexpr auto allocate(size_type n) noexcept -> pointer
    {
        if (n == 0) [[unlikely]]
        {
            return pointer();
        }
        if (n > available()) [[unlikely]]
        {
            // TODO: Log out of memory error. This allocator is not backed but
            // any other allocator, out of memory is catastrophic, use a
            // different one
            std::terminate();
        }
        auto ret = static_cast<pointer>(&buffer_[cursor_]);
        cursor_ += n;
#if ALLOCATOR_DEBUG_INITIALIZE
        std::fill(ret, ret + n, ALLOCATOR_DEBUG_INITIALIZE_VALUE);
#endif
        return ret;
    }

    constexpr auto deallocate(pointer, size_type) noexcept -> void
    {
        // Monotonic allocators do not deallocate
    }

    [[nodiscard, gnu::const]]
    static constexpr auto max_size() noexcept -> size_type
    {
        return size;
    }

    [[nodiscard]]
    constexpr auto used() const noexcept -> size_type
    {
        return cursor_;
    }

    [[nodiscard]]
    constexpr auto available() const noexcept -> size_type
    {
        return max_size() - used();
    }

private:
    value_type buffer_[size];
    size_type  cursor_{};
};

template <typename T>
    requires std::is_trivially_destructible_v<T>
class dynamic_monotonic_allocator
{
public:
    using size_type       = std::size_t;
    using value_type      = std::remove_cvref_t<T>;
    using pointer         = value_type*;
    using const_pointer   = value_type const*;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind
    {
        using other = dynamic_monotonic_allocator<U>;
    };

    dynamic_monotonic_allocator(size_type n) noexcept
        : buffer_{ new T[n] }
        , size_{ n }
    {
        assert(n > 0);
    }

    ~dynamic_monotonic_allocator() noexcept
    {
        if (buffer_ != nullptr) [[likely]]
        {
            delete[] buffer_;
            buffer_ = pointer();
        }
    }

    constexpr auto reset() noexcept -> void
    {
        cursor_ = size_type{};
    }

    [[nodiscard]]
    constexpr auto allocate(size_type n) noexcept -> pointer
    {
        if (n == 0) [[unlikely]]
        {
            return pointer();
        }
        if (n > available()) [[unlikely]]
        {
            // TODO: Log out of memory error. This allocator is not backed but
            // any other allocator, out of memory is catastrophic, use a
            // different one
            std::terminate();
        }
        auto ret = static_cast<pointer>(&buffer_[cursor_]);
        cursor_ += n;
#if ALLOCATOR_DEBUG_INITIALIZE
        std::fill(ret, ret + n, ALLOCATOR_DEBUG_INITIALIZE_VALUE);
#endif
        return ret;
    }

    constexpr auto deallocate(pointer, size_type) noexcept -> void
    {
        // Monotonic allocators do not deallocate
    }

    [[nodiscard]]
    constexpr auto max_size() const noexcept -> size_type
    {
        return size_;
    }

    [[nodiscard]]
    constexpr auto used() const noexcept -> size_type
    {
        return cursor_;
    }

    [[nodiscard]]
    constexpr auto available() const noexcept -> size_type
    {
        return max_size() - used();
    }

private:
    pointer   buffer_;
    size_type size_;
    size_type cursor_{};
};

} // namespace allocators
