#include "TApplication.h"
#include "allocator_wrapper.hpp"
#include "dynamic_array.hpp"
#include "generic_runge_kutta.hpp"
#include "operation_utils.hpp"
#include "random.hpp"
#include "runge_kutta_params.hpp"
#include "series_plot_2D.hpp"
#include "stack_allocator.hpp"
#include "static_array.hpp"
#include <cmath>
#include <iostream>
#include <numbers>

#define SEED1 104845342

template <typename F, std::size_t N>
struct nbody_system
{
    using vec_t = data_types::static_containers::static_array<F, N>;
    inline static constexpr auto epsilon = static_cast<F>(4.5e-1);
    mutable int                  iter    = 0;

    nbody_system(std::size_t n)
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
        std::cout << iter++ << dzdt << '\n';
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
            if (i == idx) continue;
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

int main()
{
    std::cout << "Hello Explicit RK world\n";

    using F               = double;
    using time_type       = F;
    constexpr auto N      = 2; // Dimension
    const auto     n      = 8; // Particles
    using Allocator       = allocators::dynamic_stack_allocator<F>;
    using StaticAllocator = allocators::static_allocator<Allocator>;
    using DVec   = data_types::dynamic_containers::dynamic_array<F, StaticAllocator>;
    using vector = data_types::static_containers::static_array<
        DVec,
        N * 2>; // * 2 Because to solve a second order differential equation with runge
                // kutta, the state needs to be pos,vel, and the derivative vel,acc.
    Allocator allocator(N * n * 2 * 10);
    StaticAllocator::set_allocator(allocator);
    utility::random::srandom::seed<F>((unsigned int)SEED1);

    const auto      dt    = F{ 0.5f };
    const time_type t0    = 0;
    const time_type t_end = 100 * std::numbers::pi_v<F>;
    vector          y0    = vector::filled(n, 0);
    const auto      k     = (int)std::ceil(t_end / dt);

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
            y0[j][i] = utility::random::srandom::randnormal(F{ 0 }, F{ 10 });
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
        y[i][0] = (float)y0[0][i];
    }

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta_base<4, 4, F, vector, vector, time_type>;

    auto   t_i   = t0;
    vector y_hat = y0;
    rk_t   stepper(
        n,
        solvers::explicit_stepers::butcher_tableau<F, 4>{
            { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
            { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
            { 0.5f, 0.5f, 1.f } }
    );
    nbody_system<F, N> s(n);
    for (auto i = 1; i != k; ++i)
    {
        stepper.do_step(s, y_hat, t_i, dt);
        t_i += dt;
        for (auto j = 0; j != n; ++j)
        {
            // std::cout << y_hat << '\n';
            y[j][i] = (float)y_hat[0][j]; // Plot the first dimension only
        }
    }

    TApplication                       app = TApplication("Root app", 0, nullptr);
    plotting::plots_2D::series_plot_2D plt(x, y, plotting::VisualizationMode::layout_2D);
    plt.render();
    app.Run();

    std::cout << "Goodbye Explicit RK world\n";
}
