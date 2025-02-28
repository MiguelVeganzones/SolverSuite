#pragma once

#include <cassert>
#include <cstddef>
#include <exception>
#include <type_traits>

namespace allocators
{

template <typename T, std::size_t N>
class static_monotonic_allocator
{
public:
    using size_type                   = std::size_t;
    using value_type                  = std::remove_cvref_t<T>;
    using pointer                     = value_type*;
    using const_pointer               = value_type const*;
    using reference                   = value_type&;
    using const_reference             = value_type const&;
    using difference_type             = std::ptrdiff_t;
    inline static constexpr auto size = N;

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
    inline constexpr auto allocate(size_type n) noexcept -> pointer
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
        return ret;
    }

    inline constexpr auto deallocate(pointer, size_type) noexcept -> void
    {
        // Monotonic allocators do not deallocate
    }

    [[nodiscard, gnu::const]]
    inline static constexpr auto max_size() noexcept -> size_type
    {
        return size;
    }

    [[nodiscard]]
    inline constexpr auto used() const noexcept -> size_type
    {
        return cursor_;
    }

    [[nodiscard]]
    inline constexpr auto available() const noexcept -> size_type
    {
        return max_size() - used();
    }

private:
    value_type buffer_[size];
    size_type  cursor_{};
};

template <typename T>
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
    inline constexpr auto allocate(size_type n) noexcept -> pointer
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
        return ret;
    }

    inline constexpr auto deallocate(pointer, size_type) noexcept -> void
    {
        // Monotonic allocators do not deallocate
    }

    [[nodiscard]]
    inline constexpr auto max_size() const noexcept -> size_type
    {
        return size_;
    }

    [[nodiscard]]
    inline constexpr auto used() const noexcept -> size_type
    {
        return cursor_;
    }

    [[nodiscard]]
    inline constexpr auto available() const noexcept -> size_type
    {
        return max_size() - used();
    }

private:
    value_type* buffer_;
    size_type   size_;
    size_type   cursor_{};
};

} // namespace allocators
