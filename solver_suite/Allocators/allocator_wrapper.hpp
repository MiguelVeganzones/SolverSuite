#pragma once

#include <cassert>
#include <cstring>

#ifndef NDEBUG

#ifdef DEBUG_PRINT
#include <iostream>
#endif

#endif

namespace allocators
{

template <typename Allocator>
class allocator_pimpl
{
public:
    using allocator_t     = Allocator;
    using size_type       = typename allocator_t::size_type;
    using value_type      = typename allocator_t::value_type;
    using pointer         = typename allocator_t::pointer;
    using const_pointer   = typename allocator_t::const_pointer;
    using reference       = typename allocator_t::reference;
    using const_reference = typename allocator_t::const_reference;
    using difference_type = typename allocator_t::difference_type;

    template <typename U>
    struct rebind
    {
        using other = typename allocator_t::template rebind<U>;
    };

    constexpr allocator_pimpl() noexcept
        : pimpl_{ nullptr }
    {
    }

    constexpr allocator_pimpl(allocator_t& impl) noexcept
        : pimpl_{ &impl }
    {
    }

    [[nodiscard]]
    constexpr auto impl() const noexcept -> allocator_t const&
    {
        assert(pimpl_ != nullptr);
        return *pimpl_;
    }

    [[nodiscard]]
    constexpr auto impl() noexcept -> allocator_t&
    {
        assert(pimpl_ != nullptr);
        return *pimpl_;
    }

    constexpr auto set_impl(allocator_t const& alloc) noexcept -> void
    {
        pimpl_ = const_cast<allocator_t*>(&alloc);
    }

    constexpr auto reset() noexcept -> void
    {
        impl().reset();
    }

    [[nodiscard]]
    constexpr auto allocate(size_type n) noexcept -> pointer
    {
        return impl().allocate(n);
    }

    constexpr auto deallocate(pointer p, size_type n) noexcept -> void
    {
        return impl().deallocate(p, n);
    }

    [[nodiscard]]
    constexpr auto max_size() const noexcept -> size_type
    {
        return impl().max_size();
    }

    [[nodiscard]]
    constexpr auto used() const noexcept -> size_type
    {
        return impl().used();
    }

    [[nodiscard]]
    constexpr auto available() const noexcept -> size_type
    {
        return impl().available();
    }

private:
    allocator_t* pimpl_;
};

template <typename Allocator>
class static_allocator
{
public:
    using allocator_t     = Allocator;
    using size_type       = typename allocator_t::size_type;
    using value_type      = typename allocator_t::value_type;
    using pointer         = typename allocator_t::pointer;
    using const_pointer   = typename allocator_t::const_pointer;
    using reference       = typename allocator_t::reference;
    using const_reference = typename allocator_t::const_reference;
    using difference_type = typename allocator_t::difference_type;

    inline static allocator_pimpl<allocator_t> s_pimpl_;

    template <typename U>
    struct rebind
    {
        using other = typename allocator_t::template rebind<U>;
    };

    static constexpr auto set_allocator(allocator_t const& pimpl) noexcept -> void
    {
        s_pimpl_.set_impl(pimpl);
    }

    constexpr auto reset() noexcept -> void
    {
        s_pimpl_.reset();
    }

    [[nodiscard]]
    constexpr auto allocate(size_type n) noexcept -> pointer
    {
        return s_pimpl_.allocate(n);
    }

    constexpr auto deallocate(pointer p, size_type n) noexcept -> void
    {
        return s_pimpl_.deallocate(p, n);
    }

    [[nodiscard]]
    constexpr auto max_size() const noexcept -> size_type
    {
        return s_pimpl_.max_size();
    }

    [[nodiscard]]
    constexpr auto used() const noexcept -> size_type
    {
        return s_pimpl_.used();
    }

    [[nodiscard]]
    constexpr auto available() const noexcept -> size_type
    {
        return s_pimpl_.available();
    }
};

} // namespace allocators
