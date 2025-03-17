#include "bm_utils.hpp"
#include "buffer_config.hpp"
#include "data_buffer.hpp"
#include "data_type_concepts.hpp"
#include "explicit_generic_runge_kutta.hpp"
#include "operation_utils.hpp"
#include "random.hpp"
#include "runge_kutta_params.hpp"
#include "static_array.hpp"
#include <benchmark/benchmark.h>
#include <cmath>
#include <iostream>
#include <numbers>

#define SEED1 104845342

template <typename F, std::size_t N>
struct nbody_system
{
    using vec_t = data_types::eagerly_evaluated_containers::static_array<F, N>;
    inline static constexpr auto epsilon = static_cast<F>(4.5e-1);

    nbody_system(std::size_t n)
        : n_{ n }
    {
    }

    inline auto operator()(
        [[maybe_unused]] auto const& z,
        [[maybe_unused]] auto&       dzdt,
        [[maybe_unused]] auto const& t
    ) -> void
    {
        for (auto i = 0uz; i != n_; ++i)
        {
            const auto acc_i = calculate_acc(z, i);
            for (auto j = 0uz; j != N; ++j)
            {
                dzdt[i, j]     = z[i, j + N];
                dzdt[i, j + N] = acc_i[j];
            }
        }
    };

    auto calculate_acc(const auto& z, std::size_t idx) const noexcept -> vec_t
    {
        vec_t d_i{};
        for (auto j = 0; j != N; ++j)
        {
            d_i[j] = z[idx, j];
        }
        vec_t d_j{};
        vec_t ret{};
        for (auto i = 0uz; i != n_; ++i)
        {
            if (i == idx) continue;
            for (auto j = 0; j != N; ++j)
            {
                d_j[j] = z[i, j];
            }
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
    using F          = double;
    using time_type  = F;
    constexpr auto N = 3;    // Dimension
    constexpr auto n = 1000; // Particles
    using buffer_t   = data_types::lazily_evaluated_containers::static_buffer<
          F,
          n,
          N * 2,
          data_types::buffer_config::LayoutPolicy::layout_row_major,
          data_types::buffer_config::layout_stride{ 0 }>;

    static_assert(data_types::dt_concepts::Indexable<buffer_t>);

    const auto      dt    = F{ 0.5f };
    const time_type t0    = 0;
    const time_type t_end = 100 * std::numbers::pi_v<F>;
    buffer_t        y0{};
    const auto      k = (int)std::ceil(t_end / dt);

    utility::random::srandom::seed<F>((unsigned int)SEED1);

    std::vector<float>              x(k);
    std::vector<std::vector<float>> y(n);
    for (auto& v : y)
    {
        v.reserve(k);
    }

    // Fill initial conditions
    for (auto i = 0uz; i != n; ++i)
    {
        for (auto j = 0uz; j != N; ++j)
        {
            y0[i, j] = utility::random::srandom::randnormal(F{ 0 }, F{ 10 });
        }
    }

    // Fill time array
    for (auto i = 0; i != k; ++i)
    {
        const auto t_i = F(i) * dt;
        x[i]           = (float)t_i;
    }
    // Fill y[0]
    for (auto i = 0; i != n; ++i)
    {
        y[i][0] = (float)y0[i, 0];
    }

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta<4, 4, F, buffer_t, buffer_t, time_type>;

    auto               t_i   = t0;
    buffer_t           y_hat = y0;
    rk_t               stepper(solvers::explicit_stepers::butcher_tableau<F, 4>{
        { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
        { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
        { 0.5f, 0.5f, 1.f } });
    nbody_system<F, N> s(n);
    for (auto _ : state)
    {
        auto               t_i   = t0;
        buffer_t           y_hat = y0;
        nbody_system<F, N> s(n);
        for (auto i = 1; i != k; ++i)
        {
            stepper.do_step(s, y_hat, t_i, dt);
            t_i += dt;
            bm_utils::escape((void*)&y_hat);
        }
    }
}

BENCHMARK(BM_ParticleSimulation_AoS);

static void BM_ParticleSimulation_SoA(benchmark::State& state)
{
    using F          = double;
    using time_type  = F;
    constexpr auto N = 3;    // Dimension
    const auto     n = 1000; // Particles
    using buffer_t   = data_types::lazily_evaluated_containers::static_buffer<
          F,
          n,
          N * 2,
          data_types::buffer_config::LayoutPolicy::layout_column_major,
          data_types::buffer_config::layout_stride{ 0 }>;

    static_assert(data_types::dt_concepts::Indexable<buffer_t>);

    const auto      dt    = F{ 0.5f };
    const time_type t0    = 0;
    const time_type t_end = 100 * std::numbers::pi_v<F>;
    buffer_t        y0{};
    const auto      k = (int)std::ceil(t_end / dt);

    utility::random::srandom::seed<F>((unsigned int)SEED1);

    std::vector<float>              x(k);
    std::vector<std::vector<float>> y(n);
    for (auto& v : y)
    {
        v.reserve(k);
    }

    // Fill initial conditions
    for (auto i = 0uz; i != n; ++i)
    {
        for (auto j = 0uz; j != N; ++j)
        {
            y0[i, j] = utility::random::srandom::randnormal(F{ 0 }, F{ 10 });
        }
    }

    // Fill time array
    for (auto i = 0; i != k; ++i)
    {
        const auto t_i = F(i) * dt;
        x[i]           = (float)t_i;
    }
    // Fill y[0]
    for (auto i = 0; i != n; ++i)
    {
        y[i][0] = (float)y0[i, 0];
    }

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta<4, 4, F, buffer_t, buffer_t, time_type>;

    auto               t_i   = t0;
    buffer_t           y_hat = y0;
    rk_t               stepper(solvers::explicit_stepers::butcher_tableau<F, 4>{
        { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
        { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
        { 0.5f, 0.5f, 1.f } });
    nbody_system<F, N> s(n);
    for (auto _ : state)
    {
        auto               t_i   = t0;
        buffer_t           y_hat = y0;
        nbody_system<F, N> s(n);
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
