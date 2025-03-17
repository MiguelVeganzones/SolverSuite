#include "TApplication.h"
#include "allocator_wrapper.hpp"
#include "dynamic_array.hpp"
#include "explicit_euler.hpp"
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
    using vector =
        data_types::lazily_evaluated_containers::dynamic_array<F, StaticAllocator>;
    Allocator allocator(1000);
    StaticAllocator::set_allocator(allocator);

    static_assert(data_types::dt_concepts::Indexable<vector>);

    const auto   dt    = F{ 0.2f };
    time_type    t0    = 0;
    time_type    t_end = 10 * std::numbers::pi_v<F>;
    const vector y0    = { std::sin(t0), std::cos(t0) };
    const auto   n     = (int)std::ceil(t_end / dt);

    std::vector<float>              x(n);
    std::vector<std::vector<float>> y(2);
    y[0].reserve(n);
    y[1].reserve(n);
    std::vector<std::vector<float>> e(6);
    e[0].reserve(n);
    e[1].reserve(n);
    e[2].reserve(n);
    e[3].reserve(n);
    e[4].reserve(n);
    e[5].reserve(n);

    for (auto i = 0; i != n; ++i)
    {
        const auto t = F(i) * dt;
        x[i]         = t;
        y[0][i]      = std::sin(t);
        y[1][i]      = std::cos(t);
    }

    using rk_t = solvers::explicit_stepers::
        generic_runge_kutta<4, 4, F, vector, vector, time_type>;
    using raltson_t = solvers::explicit_stepers::
        generic_runge_kutta<2, 2, F, vector, vector, time_type>;
    using ee_t = solvers::explicit_stepers::explicit_euler<F, vector, vector, time_type>;

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

    auto   t         = t0;
    vector y_hat_ee  = y0;
    vector y_hat_rlt = y0;
    vector y_hat_rk  = y0;
    e[0][0]          = y[0][0] - y_hat_ee[0];
    e[1][0]          = y[1][0] - y_hat_ee[1];
    e[2][0]          = y[0][0] - y_hat_rlt[0];
    e[3][0]          = y[1][0] - y_hat_rlt[1];
    e[4][0]          = y[0][0] - y_hat_rk[0];
    e[5][0]          = y[1][0] - y_hat_rk[1];

    ee_t      ee_stepper(2);
    raltson_t rtl_stepper(
        2,
        solvers::explicit_stepers::butcher_tableau<F, 2>{
            { 2.f / 3.f }, { 1.f / 4.f, 3.f / 4.f }, { 2.f / 3.f } }
    );
    rk_t rk_stepper(
        2,
        solvers::explicit_stepers::butcher_tableau<F, 4>{
            { 0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f },
            { 1.f / 6.f, 1.f / 3.f, 1.f / 3.f, 1.f / 6.f },
            { 0.5f, 0.5f, 1.f } }
    );

    for (auto i = 1; i != n; ++i)
    {
        ee_stepper.do_step(system, y_hat_ee, t, dt);
        rtl_stepper.do_step(system, y_hat_rlt, t, dt);
        rk_stepper.do_step(system, y_hat_rk, t, dt);
        t += dt;
        e[0][i] = y[0][i] - y_hat_ee[0];
        e[1][i] = y[1][i] - y_hat_ee[1];
        e[2][i] = y[0][i] - y_hat_rlt[0];
        e[3][i] = y[1][i] - y_hat_rlt[1];
        e[4][i] = y[0][i] - y_hat_rk[0];
        e[5][i] = y[1][i] - y_hat_rk[1];
    }

    TApplication                       app = TApplication("Root app", 0, nullptr);
    plotting::plots_2D::series_plot_2D plt(x, e, plotting::VisualizationMode::layout_2D);
    plt.render();
    app.Run();

    std::cout << "Goodbye Explicit RK world\n";
}
