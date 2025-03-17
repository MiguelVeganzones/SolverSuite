#include "allocator_wrapper.hpp"
#include "stack_allocator.hpp"
#include <benchmark/benchmark.h>
#include <dynamic_array.hpp>
#include <static_array.hpp>
#include <vector>

static void BM_StaticArrayAccess(benchmark::State& state)
{
    using F          = float;
    constexpr auto N = 1000uz;
    using vector     = data_types::eagerly_evaluated_containers::static_array<F, N>;
    vector v{};
    for (std::size_t i = 0; i != N; ++i)
    {
        v[i] = F(i);
    }
    for (auto _ : state)
    {
        volatile F reduced = 0;
        for (std::size_t i = 1; i != N; ++i)
        {
            reduced += v[i] - v[i - 1];
        }
    }
}

BENCHMARK(BM_StaticArrayAccess);

static void BM_DynamicVectorAccess(benchmark::State& state)
{
    using F          = float;
    constexpr auto N = 1000uz;
    using vector     = std::vector<F>;
    vector v(N);
    for (std::size_t i = 0; i != N; ++i)
    {
        v[i] = F(i);
    }
    for (auto _ : state)
    {
        volatile F reduced = 0;
        for (std::size_t i = 1; i != N; ++i)
        {
            reduced += v[i] - v[i - 1];
        }
    }
}

BENCHMARK(BM_DynamicVectorAccess);

static void BM_StaticVectorDefaultAllocAccess(benchmark::State& state)
{
    using F          = float;
    constexpr auto N = 1000uz;
    using vector     = data_types::lazily_evaluated_containers::dynamic_array<F>;
    vector v(N);
    for (std::size_t i = 0; i != N; ++i)
    {
        v[i] = F(i);
    }
    for (auto _ : state)
    {
        volatile F reduced = 0;
        for (std::size_t i = 1; i != N; ++i)
        {
            reduced += v[i] - v[i - 1];
        }
    }
}

BENCHMARK(BM_StaticVectorDefaultAllocAccess);

static void BM_StaticVectorPimplStackAllocAccess(benchmark::State& state)
{
    using F                 = float;
    constexpr auto N        = 1000uz;
    using allocator_t       = allocators::dynamic_stack_allocator<F>;
    using allocator_pimpl_t = allocators::allocator_pimpl<allocator_t>;
    using vector = data_types::lazily_evaluated_containers::dynamic_array<F, allocator_pimpl_t>;
    allocator_t allocator(N);
    vector      v(N, allocator);
    for (std::size_t i = 0; i != N; ++i)
    {
        v[i] = F(i);
    }
    for (auto _ : state)
    {
        volatile F reduced = 0;
        for (std::size_t i = 1; i != N; ++i)
        {
            reduced += v[i] - v[i - 1];
        }
    }
}

BENCHMARK(BM_StaticVectorPimplStackAllocAccess);

static void BM_StaticVectorStaticStackAllocAccess(benchmark::State& state)
{
    using F                  = float;
    constexpr auto N         = 1000uz;
    using allocator_t        = allocators::dynamic_stack_allocator<F>;
    using static_allocator_t = allocators::static_allocator<allocator_t>;
    using vector = data_types::lazily_evaluated_containers::dynamic_array<F, static_allocator_t>;
    allocator_t allocator(N);
    static_allocator_t::set_allocator(allocator);
    vector v(N);
    for (std::size_t i = 0; i != N; ++i)
    {
        v[i] = F(i);
    }
    for (auto _ : state)
    {
        volatile F reduced = 0;
        for (std::size_t i = 1; i != N; ++i)
        {
            reduced += v[i] - v[i - 1];
        }
    }
}

BENCHMARK(BM_StaticVectorStaticStackAllocAccess);

BENCHMARK_MAIN();
