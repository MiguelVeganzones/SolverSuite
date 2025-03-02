#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>

namespace allocators
{

template <std::size_t N>
class stack_allocator
{
private:
    static constexpr auto alignment = alignof(std::max_align_t);

public:
    using size_type                          = std::size_t;
    using memory_address_t                   = std::byte*;
    static constexpr size_type s_size = N;

public:
    constexpr stack_allocator() noexcept                       = default;
    constexpr stack_allocator(stack_allocator const&) noexcept = default;
    constexpr stack_allocator(stack_allocator&&) noexcept      = default;
    constexpr auto operator=(stack_allocator const&) noexcept
        -> stack_allocator&                                                  = default;
    constexpr auto operator=(stack_allocator&&) noexcept -> stack_allocator& = default;
    ~stack_allocator() noexcept                                              = default;

    constexpr auto reset() noexcept -> void
    {
        ptr_ = buffer_;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size() noexcept -> size_type
    {
        return s_size;
    }

    [[nodiscard]]
    constexpr auto used() const noexcept -> size_type
    {
        return std::distance(buffer_, ptr_);
    }

    [[nodiscard]]
    constexpr auto allocate(size_type n) noexcept -> memory_address_t
    {
        const auto aligned_n       = align_up(n);
        const auto available_bytes = static_cast<size_type>(buffer_ + N - ptr_);
        if (available_bytes >= aligned_n)
        {
            auto* ret = ptr_;
            ptr_ += aligned_n;
            return ret;
        }
        else
        {
            return static_cast<std::byte*>(::operator new(n));
        }
    }

    constexpr auto deallocate(memory_address_t p, std::size_t n) noexcept -> void
    {
        if (pointer_in_buffer(p))
        {
            n = align_up(n);
            if (p + n == ptr_)
            {
                ptr_ = p;
            }
        }
        else
        {
            ::operator delete(p);
        }
    }

private:
    [[nodiscard, gnu::const]]
    static constexpr auto align_up(size_type n) noexcept -> size_type
    {
        return (n + (alignment - 1) & ~(alignment - 1));
    }

    [[nodiscard, gnu::const]]
    constexpr auto pointer_in_buffer(const memory_address_t p) -> bool
    {
        return std::uintptr_t(p) >= std::uintptr_t(buffer_) &&
               std::uintptr_t(p) < std::uintptr_t(buffer_) + s_size;
    }

private:
    alignas(alignment) std::byte buffer_[size];
    memory_address_t ptr_{};
};

} // namespace allocators
