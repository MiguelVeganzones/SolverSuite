#include "allocator_wrapper.hpp"
#include "bm_utils.hpp"
#include "dynamic_array.hpp"
#include "explicit_generic_runge_kutta.hpp"
#include "operation_utils.hpp"
#include "random.hpp"
#include "runge_kutta_params.hpp"
#include "stack_allocator.hpp"
#include "static_array.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

// SoA is slowe because dynamic vector operations are evaluated eagerly

#define T_END 250
#define DT 0.5f
#define SEED1 104845342
#define float_value_type float;
constexpr auto N = 3;  // Dimension
constexpr auto n = 24; // Particles

template <typename F, std::size_t N>
struct nbody_system_AoS
{
    using vec_t = data_types::eagerly_evaluated_containers::static_array<F, N>;
    inline static constexpr auto epsilon = static_cast<F>(4.5e-1);

    // mutable int                  iter    = 0;

    nbody_system_AoS(std::size_t n)
        : n_{ n }
    {
    }

    inline auto operator()(
        [[maybe_unused]] auto const& z,
        [[maybe_unused]] auto&       dzdt,
        [[maybe_unused]] auto const& t
    ) const -> void
    {
        for (auto i = 0uz; i != n_; ++i)
        {
            const auto acc_i = calculate_acc(z, i);
            for (auto j = 0uz; j != N; ++j)
            {
                dzdt[i][j]     = z[i][j + N];
                dzdt[i][j + N] = acc_i[j];
            }
        }
    };

    auto calculate_acc(const auto& z, std::size_t idx) const noexcept -> vec_t
    {
        vec_t d_i = z[idx].template slice<vec_t>(0, N);
        vec_t d_j{};
        vec_t ret{};
        for (auto i = 0uz; i != n_; ++i)
        {
            if (i == idx) [[unlikely]]
            {
                continue;
            }
            d_j                 = z[i].template slice<vec_t>(0, N);
            const auto distance = data_types::operation_utils::distance(d_i, d_j);
            const auto d        = data_types::operation_utils::l2_norm(distance);
            ret += distance / (d * d * d + epsilon);
        }
        return ret;
    }

    std::size_t n_;
};

static void BM_ParticleSimulation_AoS(benchmark::State& state)
{
    using F         = float_value_type;
    using time_type = F;
    using SVec      = data_types::eagerly_evaluated_containers::static_array<
             F,
             N * 2>; // * 2 Because to solve a second order differential equation with runge
                     // kutta, the state needs to be pos,vel, and the derivative vel,acc.
    using Allocator       = allocators::dynamic_stack_allocator<SVec>;
    using StaticAllocator = allocators::static_allocator<Allocator>;
    using vector = data_types::lazily_evaluated_containers::dynamic_array<SVec, StaticAllocator>;
    Allocator allocator(N * n * 2 * 10);
    StaticAllocator::set_allocator(allocator);
    utility::random::srandom::seed<F>((unsigned int)SEED1);

    const auto dt    = F{ DT };
    time_type  t0    = 0;
    time_type  t_end = T_END;
    vector     y0(n, SVec{});
    const auto k = (int)std::ceil(t_end / dt);

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta<4, 4, F, vector, vector, time_type>;
    rk_t stepper(
        n,
        solvers::explicit_stepers::butcher_tableau<F, 4>{
            { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
            { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
            { 0.5f, 0.5f, 1.f } }
    );

    // Fill initial conditions
    for (auto i = 0uz; i != n; ++i)
    {
        for (auto j = 0uz; j != N; ++j)
        {
            y0[i][j] = utility::random::srandom::randnormal(F{ 0 }, F{ 10 });
        }
    }

    for (auto _ : state)
    {
        auto                   t_i   = t0;
        vector                 y_hat = y0;
        nbody_system_AoS<F, N> s(n);
        for (auto i = 1; i != k; ++i)
        {
            stepper.do_step(s, y_hat, t_i, dt);
            t_i += dt;
            bm_utils::escape((void*)&y_hat);
        }
    }
}

BENCHMARK(BM_ParticleSimulation_AoS);

template <typename F, std::size_t N>
struct nbody_system_SoA
{
    using vec_t = data_types::eagerly_evaluated_containers::static_array<F, N>;
    inline static constexpr auto epsilon = static_cast<F>(4.5e-1);

    nbody_system_SoA(std::size_t n)
        : n_{ n }
    {
    }

    inline auto operator()(
        [[maybe_unused]] auto const& z,
        [[maybe_unused]] auto&       dzdt,
        [[maybe_unused]] auto const& t
    ) const -> void
    {
        for (auto i = 0uz; i != n_; ++i)
        {
            const auto acc_i = calculate_acc(z, i);
            for (auto j = 0uz; j != N; ++j)
            {
                dzdt[j][i]     = z[j + N][i];
                dzdt[j + N][i] = acc_i[j];
            }
        }
    };

    auto calculate_acc(const auto& z, std::size_t idx) const noexcept -> vec_t
    {
        vec_t d_i = {};
        for (auto k = 0uz; k != N; ++k)
        {
            d_i[k] = z[k][idx];
        }
        vec_t d_j{};
        vec_t ret{};
        for (auto i = 0uz; i != n_; ++i)
        {
            if (i == idx) [[unlikely]]
            {
                continue;
            }
            for (auto k = 0uz; k != N; ++k)
            {
                d_j[k] = z[k][i];
            }
            const auto distance = data_types::operation_utils::distance(d_i, d_j);
            const auto d        = data_types::operation_utils::l2_norm(distance);
            ret += distance / (d * d * d + epsilon);
        }
        return ret;
    }

    std::size_t n_;
};

static void BM_ParticleSimulation_SoA(benchmark::State& state)
{
    using F               = float_value_type;
    using time_type       = F;
    using Allocator       = allocators::dynamic_stack_allocator<F>;
    using StaticAllocator = allocators::static_allocator<Allocator>;
    using DVec   = data_types::lazily_evaluated_containers::dynamic_array<F, StaticAllocator>;
    using vector = data_types::eagerly_evaluated_containers::static_array<
        DVec,
        N * 2>; // * 2 Because to solve a second order differential equation with runge
                // kutta, the state needs to be pos,vel, and the derivative vel,acc.
    Allocator allocator(N * n * 2 * 10);
    StaticAllocator::set_allocator(allocator);
    utility::random::srandom::seed<F>((unsigned int)SEED1);

    const auto dt    = F{ DT };
    time_type  t0    = 0;
    time_type  t_end = T_END;
    vector     y0    = vector::filled(n, 0);
    const auto k     = (int)std::ceil(t_end / dt);

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta<4, 4, F, vector, vector, time_type>;
    rk_t stepper(
        n,
        solvers::explicit_stepers::butcher_tableau<F, 4>{
            { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
            { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
            { 0.5f, 0.5f, 1.f } }
    );

    // Fill initial conditions
    for (auto i = 0uz; i != n; ++i)
    {
        for (auto j = 0uz; j != N; ++j)
        {
            y0[j][i] = utility::random::srandom::randnormal(F{ 0 }, F{ 10 });
        }
    }

    for (auto _ : state)
    {
        auto                   t_i   = t0;
        vector                 y_hat = y0;
        nbody_system_SoA<F, N> s(n);
        for (auto i = 1; i != k; ++i)
        {
            stepper.do_step(s, y_hat, t_i, dt);
            t_i += dt;
            bm_utils::escape((void*)&y_hat);
        }
    }
}

BENCHMARK(BM_ParticleSimulation_SoA);

BENCHMARK_MAIN();
