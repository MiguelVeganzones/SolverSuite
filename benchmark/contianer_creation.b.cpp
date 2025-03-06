#include "allocator_wrapper.hpp"
#include "bm_utils.hpp"
#include "stack_allocator.hpp"
#include <benchmark/benchmark.h>
#include <dynamic_array.hpp>
#include <static_array.hpp>
#include <vector>

static void BM_StaticArrayAccess(benchmark::State& state)
{
    using F          = float;
    constexpr auto N = 1000uz;
    using vector     = data_types::static_containers::static_array<F, N>;
    for (auto _ : state)
    {
        vector v{};
        bm_utils::escape(v.data());
    }
}

BENCHMARK(BM_StaticArrayAccess);

static void BM_DynamicVectorAccess(benchmark::State& state)
{
    using F          = float;
    constexpr auto N = 1000uz;
    using vector     = std::vector<F>;
    for (auto _ : state)
    {
        vector v(N);
        bm_utils::escape(v.data());
    }
}

BENCHMARK(BM_DynamicVectorAccess);

static void BM_StaticVectorDefaultAllocAccess(benchmark::State& state)
{
    using F          = float;
    constexpr auto N = 1000uz;
    using vector     = data_types::dynamic_containers::dynamic_array<F>;
    for (auto _ : state)
    {
        vector v(N);
        bm_utils::escape(v.data());
    }
}

BENCHMARK(BM_StaticVectorDefaultAllocAccess);

static void BM_StaticVectorPimplStackAllocAccess(benchmark::State& state)
{
    using F                 = float;
    constexpr auto N        = 1000uz;
    using allocator_t       = allocators::dynamic_stack_allocator<F>;
    using allocator_pimpl_t = allocators::allocator_pimpl<allocator_t>;
    using vector = data_types::dynamic_containers::dynamic_array<F, allocator_pimpl_t>;
    for (auto _ : state)
    {
        allocator_t allocator(N);
        vector      v(N, allocator);
        bm_utils::escape(v.data());
    }
}

BENCHMARK(BM_StaticVectorPimplStackAllocAccess);

static void BM_StaticVectorStaticStackAllocAccess(benchmark::State& state)
{
    using F                  = float;
    constexpr auto N         = 1000uz;
    using allocator_t        = allocators::dynamic_stack_allocator<F>;
    using static_allocator_t = allocators::static_allocator<allocator_t>;
    using vector = data_types::dynamic_containers::dynamic_array<F, static_allocator_t>;
    for (auto _ : state)
    {
        allocator_t allocator(N);
        static_allocator_t::set_allocator(allocator);
        vector v(N);
    }
}

BENCHMARK(BM_StaticVectorStaticStackAllocAccess);

BENCHMARK_MAIN();
