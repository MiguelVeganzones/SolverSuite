#include "TApplication.h"
#include "buffer_config.hpp"
#include "data_buffer.hpp"
#include "data_type_concepts.hpp"
#include "explicit_generic_embedded_runge_kutta.hpp"
#include "operation_utils.hpp"
#include "random.hpp"
#include "runge_kutta_params.hpp"
#include "series_plot_2D.hpp"
#include "static_array.hpp"
#include <cmath>
#include <iostream>
#include <numbers>

#define SEED1 104845342

template <typename F, std::size_t N>
struct nbody_system
{
    using vec_t = data_types::eagerly_evaluated_containers::static_array<F, N>;
    inline static constexpr auto epsilon = static_cast<F>(4.5e-1);
    int                          iter    = 0;

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
        // std::cout << iter++ << dzdt << '\n';
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

int main()
{
    std::cout << "Hello Explicit RK world\n";

    using F          = double;
    using time_type  = F;
    constexpr auto N = 2; // Dimension
    const auto     n = 8; // Particles
    using buffer_t   = data_types::lazily_evaluated_containers::static_buffer<
          F,
          n,
          N * 2,
          data_types::buffer_config::LayoutPolicy::layout_column_major,
          data_types::buffer_config::layout_stride{ 0 }>;

    static_assert(data_types::dt_concepts::Indexable<buffer_t>);

    const time_type t0    = 0;
    const time_type t_end = 100 * std::numbers::pi_v<F>;
    buffer_t        y0{};

    utility::random::srandom::seed<F>((unsigned int)SEED1);

    std::vector<float>              x;
    std::vector<std::vector<float>> y(n);

    // Fill initial conditions
    for (auto i = 0uz; i != n; ++i)
    {
        for (auto j = 0uz; j != N; ++j)
        {
            y0[i, j] = utility::random::srandom::randnormal(F{ 0 }, F{ 10 });
        }
    }

    // Fill y[0]
    for (auto i = 0; i != n; ++i)
    {
        y[i].push_back((float)y0[i, 0]);
    }

    using rkf_t = solvers::explicit_stepers::
        explicit_embedded_runge_kutta<6, 4, 5, 5, F, buffer_t, buffer_t, time_type>;

    auto               t_i   = t0;
    buffer_t           y_hat = y0;
    rkf_t              stepper(solvers::explicit_stepers::extended_butcher_tableau<F, 6>(
        { F(0.25),
                       F(3.0 / 32.0),
                       F(9.0 / 32.0),
                       F(1932.0 / 2197.0),
                       F(-7200.0 / 2197.0),
                       F(7296.0 / 2197.0),
                       F(439.0 / 216.0),
                       F(-8.0),
                       F(3680.0 / 513.0),
                       F(-845.0 / 4104.0),
                       F(-8.0 / 27.0),
                       F(2),
                       F(-3544.0 / 2565.0),
                       F(1859.0 / 4104.0),
                       F(-11.0 / 40.0) },
        { F(16.0 / 135.0),
                       F(0),
                       F(6656.0 / 12825.0),
                       F(28561.0 / 56430.0),
                       F(-9.0 / 50.0),
                       F(2.0 / 55.0) },
        { F(25.0 / 216.0),
                       F(0),
                       F(1408.0 / 2565.0),
                       F(2197.0 / 4104.0),
                       F(-1.0 / 5.0),
                       F(0) },
        { F(0.25), F(3.0 / 8.0), F(12.0 / 13.0), F(1), F(0.5) }
    ));
    nbody_system<F, N> s(n);
    while (t_i < t_end)
    {
        stepper.do_step_impl(s, y_hat, t_i);
        x.push_back((float)t_i);
        std::cout << t_i << '\n';
        for (auto j = 0; j != n; ++j)
        {
            // std::cout << y_hat << '\n';
            y[j].push_back((float)y_hat[j, 0]); // Plot the first dimension only
        }
    }

    TApplication                       app = TApplication("Root app", 0, nullptr);
    plotting::plots_2D::series_plot_2D plt(x, y, plotting::VisualizationMode::layout_2D);
    plt.render();
    app.Run();

    std::cout << "Goodbye Explicit RK world\n";
}
