#include "TApplication.h"
#include "allocator_wrapper.hpp"
#include "dynamic_array.hpp"
#include "explicit_generic_runge_kutta.hpp"
#include "runge_kutta_params.hpp"
#include "series_plot_2D.hpp"
#include "stack_allocator.hpp"
#include <cmath>
#include <iostream>
#include <numbers>

int main()
{
    std::cout << "Hello Explicit RK world\n";

    // x'' = -x
    // x_1 = x
    // x_2 = x'
    // (x_1; x_2)' = (0, 1; -1, 0)(x_1; x_2)

    using F               = float;
    using time_type       = F;
    using Allocator       = allocators::dynamic_stack_allocator<F>;
    using StaticAllocator = allocators::static_allocator<Allocator>;
    using vector = data_types::lazily_evaluated_containers::dynamic_array<F, StaticAllocator>;
    Allocator allocator(100);
    StaticAllocator::set_allocator(allocator);

    const auto   dt    = F{ 0.1f };
    time_type    t0    = 0;
    time_type    t_end = 10 * std::numbers::pi_v<F>;
    const vector y0    = { std::sin(t0), std::cos(t0) };
    const auto   n     = (int)std::ceil(t_end / dt);

    std::vector<float>              x(n);
    std::vector<std::vector<float>> y(2);
    y[0].reserve(n);
    y[1].reserve(n);

    for (auto i = 0; i != n; ++i)
    {
        const auto t = F(i) * dt;
        x[i]         = t;
        y[0][i]      = std::sin(t);
    }

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta<4, 4, F, vector, vector, time_type>;

    auto system = [](auto const& z,
                     auto&       dzdt,
                     [[maybe_unused]]
                     auto const& t) -> void {
        // x'' = -x
        // x_1 = x
        // x_2 = x'
        // (x_1; x_2)' = (0, 1; -1, 0)(x_1; x_2)
        dzdt[0] = z[1];
        dzdt[1] = -z[0];
    };

    auto   t     = t0;
    vector y_hat = y0;
    y[1][0]      = y_hat[0];
    rk_t stepper(
        2,
        solvers::explicit_stepers::butcher_tableau<F, 4>{
            { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
            { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
            { 0.5f, 0.5f, 1.f } }
    );
    for (auto i = 1; i != n; ++i)
    {
        stepper.do_step(system, y_hat, t, dt);
        t += dt;
        y[1][i] = y_hat[0];
    }

    TApplication                       app = TApplication("Root app", 0, nullptr);
    plotting::plots_2D::series_plot_2D plt(x, y, plotting::VisualizationMode::layout_2D);
    plt.render();
    app.Run();

    std::cout << "Goodbye Explicit RK world\n";
}
