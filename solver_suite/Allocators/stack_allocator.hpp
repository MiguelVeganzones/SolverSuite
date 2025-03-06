#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <type_traits>

#ifndef NDEBUG

#ifdef DEBUG_PRINT
#include <iostream>
#endif

#ifndef ALLOCATOR_DEBUG_NAN_INITIALIZE
#define ALLOCATOR_DEBUG_INITIALIZE 1
#if ALLOCATOR_DEBUG_INITIALIZE
#include <algorithm>
#define ALLOCATOR_DEBUG_INITIALIZE_VALUE std::byte{ 0x80 }
#define ALLOCATOR_DEBUG_DEFAULT_ALLOCATE_VALUE std::byte{ 0xFF }
#endif
#endif

#endif

namespace allocators
{

template <typename T>
    requires std::is_trivially_destructible_v<T>
class dynamic_stack_allocator
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
        using other = dynamic_stack_allocator<U>;
    };

    constexpr dynamic_stack_allocator(size_type n) noexcept
        : buffer_{ new T[n] }
        , size_{ n }
    {
        assert(n > 0);
#if ALLOCATOR_DEBUG_INITIALIZE
        std::fill_n(
            (std::byte*)buffer_, size_ * sizeof(T), ALLOCATOR_DEBUG_INITIALIZE_VALUE
        );
#endif
#ifdef DEBUG_PRINT
        for (auto i = 0uz; i != size_; ++i)
        {
            std::cout << buffer_[i];
        }
        std::cout << '\n';
#endif
    }

    ~dynamic_stack_allocator() noexcept
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
#ifdef DEBUG_PRINT
        std::cout << "Allocating " << n << " elements. Total size: " << n * sizeof(T)
                  << '\n';
#endif
        if (n == 0) [[unlikely]]
        {
            return pointer{};
        }
        pointer ret;
        if (n > available()) [[unlikely]]
        {
            ret = new value_type[n];
        }
        else
        {
            ret = static_cast<pointer>(&buffer_[cursor_]);
            cursor_ += n;
        }
#if ALLOCATOR_DEBUG_INITIALIZE
        std::fill_n(
            (std::byte*)ret, n * sizeof(T), ALLOCATOR_DEBUG_DEFAULT_ALLOCATE_VALUE
        );
#endif
        return ret;
    }

    constexpr auto deallocate(pointer p, size_type n) noexcept -> void
    {
#ifdef DEBUG_PRINT
        std::cout << "Deallocating " << n << " elements at " << p
                  << ". Total size: " << n * sizeof(T) << '\n';
#endif
        if (pointer_in_buffer(p))
        {
#ifdef DEBUG_PRINT
            std::cout << "Deallocated from buffer\n";
#endif
            if (&p[n] == &buffer_[cursor_])
            {
                [[assume(cursor_ >= n)]];
                cursor_ -= n;
            }
        }
        else
        {
#ifdef DEBUG_PRINT
            std::cout << "Backup allocator dealocates\n";
#endif
            ::operator delete(p);
        }
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
    [[nodiscard]]
    constexpr auto pointer_in_buffer(const_pointer p) const -> bool
    {
        return std::less_equal<const_pointer>{}(buffer_, p) &&
               std::less<const_pointer>{}(p, buffer_ + size_);
    }

private:
    pointer   buffer_;
    size_type size_;
    size_type cursor_{};
};

} // namespace allocators
