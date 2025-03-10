#pragma once

#include "data_type_concepts.hpp"
#include "explicit_stepper_base.hpp"
#include "operation_utils.hpp"
#include "runge_kutta_params.hpp"
#include <cassert>
#include <concepts>
#include <limits>

namespace solvers::explicit_stepers
{

template <
    std::size_t         Stage_Count,
    std::size_t         Order,
    std::floating_point Value_Type,
    typename State_Type,
    typename Deriv_Type,
    typename Time_Type>
class generic_runge_kutta_base : public explicit_stepers_base<
                                     generic_runge_kutta_base<
                                         Stage_Count,
                                         Order,
                                         Value_Type,
                                         State_Type,
                                         Deriv_Type,
                                         Time_Type>,
                                     Order,
                                     Value_Type,
                                     State_Type,
                                     Deriv_Type,
                                     Time_Type>
{
private:
    inline static constexpr auto s_stage_count = (int)Stage_Count;

public:
    using size_type      = std::size_t;
    using value_type     = Value_Type;
    using state_type     = State_Type;
    using deriv_type     = Deriv_Type;
    using time_type      = Time_Type;
    using rk_params_type = butcher_tableau<value_type, s_stage_count>;

public:
    constexpr generic_runge_kutta_base(rk_params_type rk_params) noexcept
        : m_rk_params{ rk_params }
    {
    }

    constexpr generic_runge_kutta_base(
        size_type             n,
        rk_params_type const& rk_params
    ) noexcept
        : m_rk_params{ rk_params }
    {
        resize_internals(n);
        /*
        std::cout << m_x_tmp.cbegin() << '\n';
        std::cout << m_x_tmp.cend() << '\n';
        for (auto& v : m_dxdt)
        {
            std::cout << v.cbegin() << '\n';
            std::cout << v.cend() << '\n';
        }
        */
    }

    [[nodiscard]]
    static constexpr auto stage_count() noexcept -> size_type
    {
        return Stage_Count;
    }

    auto do_step_impl(
        auto&&      system,
        state_type& x_in_out,
        time_type   t,
        time_type   dt
    ) noexcept -> void
    {
#ifndef NDEBUG
        if constexpr (data_types::dt_concepts::SizedInstance<deriv_type>)
        {
            assert(x_in_out.size() == m_x_tmp.size());
        }
        if constexpr (data_types::dt_concepts::SizedInstance<deriv_type> &&
                      data_types::dt_concepts::SizedInstance<state_type>)
        {
            for (auto const& dx : m_dxdt)
            {
                assert(x_in_out.size() == dx.size());
            }
        }
#endif

        system(x_in_out, m_dxdt[0], t);
        for (auto j = 1; j < s_stage_count; ++j)
        {
            const auto t_j = t + m_rk_params.c(j) * dt;
            m_x_tmp        = x_in_out;
            for (auto i = 0; i < j; ++i)
            {
                const auto a = m_rk_params.a(j, i);
                if (std::abs(a) < std::numeric_limits<value_type>::epsilon()) continue;
                m_x_tmp += m_dxdt[i] * (a * dt);
            }
            system(m_x_tmp, m_dxdt[j], t_j);
        }
        /*
        const auto& b = m_rk_params.b();
        x_in_out += dt * expr_reduce<Stage_Count>(m_dxdt, b);
        */
        x_in_out += dt * result_expr();
        // x_in_out += dt * (m_dxdt[0] * b[0] + m_dxdt[1] * b[1] + m_dxdt[2] * b[2] +
        // m_dxdt[3] * b[3]);
    }

    [[nodiscard]]
    constexpr auto result_expr() const noexcept -> auto const&
        requires data_types::dt_concepts::LazilyEvaluatedExpr<deriv_type, value_type>
    {
        static const auto expr = data_types::operation_utils::expr_reduce<Stage_Count>(
            m_dxdt, m_rk_params.b()
        );
        return expr;
    }

    [[nodiscard]]
    constexpr auto result_expr() const noexcept -> auto
        requires data_types::dt_concepts::EagerlyEvaluatedExpr<deriv_type, value_type>
    {
        return data_types::operation_utils::expr_reduce<Stage_Count>(
            m_dxdt, m_rk_params.b()
        );
    }

    auto resize_internals(size_type n) noexcept -> void
        requires data_types::dt_concepts::Resizeable<deriv_type> ||
                 data_types::dt_concepts::Resizeable<typename deriv_type::value_type> ||
                 data_types::dt_concepts::Resizeable<state_type> ||
                 data_types::dt_concepts::Resizeable<typename state_type::value_type>
    {
        assert(n > 0);
        if constexpr (data_types::dt_concepts::Resizeable<state_type>)
        {
            m_x_tmp.resize(n);
        }
        else if constexpr (data_types::dt_concepts::Resizeable<
                               typename state_type::value_type> &&
                           std::ranges::range<state_type>)
        {
            for (auto& e : m_x_tmp)
            {
                e.resize(n);
            }
        }
        if constexpr (data_types::dt_concepts::Resizeable<deriv_type>)
        {
            for (auto& dx : m_dxdt)
            {
                dx.resize(n);
            }
        }
        else if constexpr (data_types::dt_concepts::Resizeable<
                               typename state_type::value_type> &&
                           std::ranges::range<deriv_type>)
        {
            for (auto& dx : m_dxdt)
            {
                for (auto& e : dx)
                {
                    e.resize(n);
                }
            }
        }
    }

private:
    alignas(64) rk_params_type m_rk_params;
    alignas(64) state_type m_x_tmp;
    alignas(64) deriv_type m_dxdt[Stage_Count];
};

} // namespace solvers::explicit_stepers
