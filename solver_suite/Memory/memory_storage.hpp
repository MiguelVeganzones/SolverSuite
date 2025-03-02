#pragma once

#include "monotonic_allocator.hpp"
#include <array>
#include <cstddef>

namespace buffers::memory_storage
{

template <typename T, std::size_t K, std::size_t N>
class static_uniform_multipool
{
    inline static constexpr auto num_pools  = K;
    inline static constexpr auto pool_size  = N;
    inline static constexpr auto total_size = num_pools * pool_size;
    using impl_allocator_t = allocators::static_monotonic_allocator<T, total_size>;

    using index_t         = std::size_t;
    using size_type       = typename impl_allocator_t::size_type;
    using value_type      = typename impl_allocator_t::value_type;
    using pointer         = typename impl_allocator_t::pointer;
    using const_pointer   = typename impl_allocator_t::const_pointer;
    using reference       = typename impl_allocator_t::reference;
    using const_reference = typename impl_allocator_t::const_reference;
    using difference_type = typename impl_allocator_t::difference_type;

    constexpr static_uniform_multipool() noexcept
    {
        for (auto& p : pools_)
        {
            p = allocator_.allocate(pool_size);
            assert(p != nullptr);
        }
    }

    [[nodiscard]]
    inline constexpr auto buffer(index_t idx) const noexcept -> pointer
    {
        assert(idx < num_pools);
        return pools_[idx];
    }

    impl_allocator_t               allocator_{};
    std::array<pointer, num_pools> pools_;
};

template <typename T, std::size_t K>
class dynamic_uniform_multipool
{
    inline static constexpr auto num_pools = K;
    using impl_allocator_t                 = allocators::dynamic_monotonic_allocator<T>;

    using index_t         = std::size_t;
    using size_type       = typename impl_allocator_t::size_type;
    using value_type      = typename impl_allocator_t::value_type;
    using pointer         = typename impl_allocator_t::pointer;
    using const_pointer   = typename impl_allocator_t::const_pointer;
    using reference       = typename impl_allocator_t::reference;
    using const_reference = typename impl_allocator_t::const_reference;
    using difference_type = typename impl_allocator_t::difference_type;

public:
    constexpr dynamic_uniform_multipool(size_type pool_size) noexcept
        : allocator_{ pool_size * K }
        , pool_size_{ pool_size }
    {
        for (auto& p : pools_)
        {
            p = allocator_.allocate(pool_size);
            assert(p != nullptr);
        }
    }

    [[nodiscard]]
    inline constexpr auto pool(index_t idx) const noexcept -> pointer
    {
        assert(idx < num_pools);
        return pools_[idx];
    }

private:
    impl_allocator_t               allocator_{};
    std::array<pointer, num_pools> pools_;
    size_type                      pool_size_;
};

} // namespace buffers::memory_storage
