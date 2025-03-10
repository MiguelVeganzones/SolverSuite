#include "allocator_wrapper.hpp"
#include "bm_utils.hpp"
#include "dynamic_array.hpp"
#include "generic_runge_kutta.hpp"
#include "operation_utils.hpp"
#include "random.hpp"
#include "runge_kutta_params.hpp"
#include "stack_allocator.hpp"
#include "static_array.hpp"
#include <cmath>
#include <iostream>

#define SEED1 104845342

template <typename F, std::size_t N>
struct nbody_system
{
    using vec_t = data_types::static_containers::static_array<F, N>;
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
                dzdt[i][j] = z[i][j + N];
            }
            for (auto j = 0uz; j != N; ++j)
            {
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

int main()
{
    using F          = float;
    using time_type  = F;
    constexpr auto N = 3;    // Dimension
    const auto     n = 1000; // Particles
    using SVec       = data_types::static_containers::static_array<
              F,
              N * 2>; // * 2 Because to solve a second order differential equation with runge
                      // kutta, the state needs to be pos,vel, and the derivative vel,acc.
    using Allocator       = allocators::dynamic_stack_allocator<SVec>;
    using StaticAllocator = allocators::static_allocator<Allocator>;
    using vector = data_types::dynamic_containers::dynamic_array<SVec, StaticAllocator>;
    Allocator allocator(N * n * 2 * 10);
    StaticAllocator::set_allocator(allocator);
    utility::random::srandom::seed<F>((unsigned int)SEED1);

    const auto dt    = F{ 0.5f };
    time_type  t0    = 0;
    time_type  t_end = 250;
    vector     y0(n, SVec{});
    const auto k = (int)std::ceil(t_end / dt);

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta_base<4, 4, F, vector, vector, time_type>;
    rk_t stepper(
        n,
        solvers::explicit_stepers::butcher_tableau<F, 4>{
            { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
            { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
            { 0.5f, 0.5f, 1.f } }
    );

    // Fill initial conditions
    std::generate(std::begin(y0), std::end(y0), []() mutable {
        SVec ret;
        std::generate(std::begin(ret), std::begin(ret) + N, []() mutable {
            return utility::random::srandom::randnormal(F{ 0 }, F{ 10 });
        });
        return ret;
    });

    auto               t_i   = t0;
    vector             y_hat = y0;
    nbody_system<F, N> s(n);
    for (auto i = 1; i != k; ++i)
    {
        stepper.do_step(s, y_hat, t_i, dt);
        t_i += dt;
        bm_utils::escape((void*)&y_hat);
    }
    std::cout << y_hat[0];
}
