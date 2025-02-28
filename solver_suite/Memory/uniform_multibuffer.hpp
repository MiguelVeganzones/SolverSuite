#pragma once

#include "monotonic_allocator.hpp"
#include <array>
#include <cstddef>

namespace buffers::multibuffer
{

template <typename T, std::size_t K, std::size_t N>
class static_uniform_multibuffer
{
    inline static constexpr auto num_buffers = K;
    inline static constexpr auto buffer_size = N;
    inline static constexpr auto total_size  = num_buffers * buffer_size;
    using impl_allocator_t = allocators::static_monotonic_allocator<T, total_size>;

    using index_t         = std::size_t;
    using size_type       = typename impl_allocator_t::size_type;
    using value_type      = typename impl_allocator_t::value_type;
    using pointer         = typename impl_allocator_t::pointer;
    using const_pointer   = typename impl_allocator_t::const_pointer;
    using reference       = typename impl_allocator_t::reference;
    using const_reference = typename impl_allocator_t::const_reference;
    using difference_type = typename impl_allocator_t::difference_type;

    constexpr static_uniform_multibuffer() noexcept
    {
        for (auto* p : buffers_)
        {
            p = allocator_.allocate(buffer_size);
            assert(p != nullptr);
        }
    }

    [[nodiscard]]
    inline constexpr auto buffer(index_t idx) const noexcept -> pointer
    {
        assert(idx < num_buffers);
        return buffers_[idx];
    }

    impl_allocator_t                 allocator_{};
    std::array<pointer, num_buffers> buffers_;
};

template <typename T, std::size_t K>
class dynamic_uniform_multibuffer
{
    inline static constexpr auto num_buffers = K;
    using impl_allocator_t                   = allocators::dynamic_monotonic_allocator<T>;

    using index_t         = std::size_t;
    using size_type       = typename impl_allocator_t::size_type;
    using value_type      = typename impl_allocator_t::value_type;
    using pointer         = typename impl_allocator_t::pointer;
    using const_pointer   = typename impl_allocator_t::const_pointer;
    using reference       = typename impl_allocator_t::reference;
    using const_reference = typename impl_allocator_t::const_reference;
    using difference_type = typename impl_allocator_t::difference_type;

public:
    constexpr dynamic_uniform_multibuffer(size_type pool_size) noexcept
        : allocator_{ pool_size * K }
        , buffer_size_{ pool_size }
    {
        for (auto* p : buffers_)
        {
            p = allocator_.allocate(pool_size);
            assert(p != nullptr);
        }
    }

    [[nodiscard]]
    inline constexpr auto buffer(index_t idx) const noexcept -> pointer
    {
        assert(idx < num_buffers);
        return buffers_[idx];
    }

private:
    impl_allocator_t                 allocator_{};
    std::array<pointer, num_buffers> buffers_;
    size_type                        buffer_size_;
};

} // namespace buffers::multibuffer
