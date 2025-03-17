#pragma once

#include "data_type_concepts.hpp"
#include "explicit_stepper_base.hpp"
#include "operation_utils.hpp"
#include "runge_kutta_params.hpp"
#include <concepts>
#include <cstdint>

namespace solvers::explicit_stepers
{

enum struct ControlledStepResult
{
    failure,
    success,
};

template <
    std::uint8_t        Stage_Count,
    std::uint8_t        Stepper_Order,
    std::uint8_t        Error_Stepper_Order,
    std::uint8_t        Error_Order,
    std::floating_point Value_Type,
    typename State_Type,
    typename Deriv_Type,
    typename Time_Type>
class explicit_embedded_runge_kutta : explicit_stepers_base<
                                          explicit_embedded_runge_kutta<
                                              Stage_Count,
                                              Stepper_Order,
                                              Error_Stepper_Order,
                                              Error_Order,
                                              Value_Type,
                                              State_Type,
                                              Deriv_Type,
                                              Time_Type>,
                                          Stepper_Order,
                                          Value_Type,
                                          State_Type,
                                          Deriv_Type,
                                          Time_Type>
{
public:
    using stepper_base_type = explicit_stepers_base<
        explicit_embedded_runge_kutta<
            Stage_Count,
            Stepper_Order,
            Error_Stepper_Order,
            Error_Order,
            Value_Type,
            State_Type,
            Deriv_Type,
            Time_Type>,
        Stepper_Order,
        Value_Type,
        State_Type,
        Deriv_Type,
        Time_Type>;
    using size_type      = typename stepper_base_type::size_type;
    using order_type     = typename stepper_base_type::order_type;
    using value_type     = Value_Type;
    using state_type     = State_Type;
    using deriv_type     = Deriv_Type;
    using time_type      = Time_Type;
    using rk_params_type = extended_butcher_tableau<value_type, Stage_Count>;

private:
    inline static constexpr auto s_stage_count = static_cast<order_type>(Stage_Count);
    inline static constexpr auto s_error_stepper_order =
        static_cast<order_type>(Error_Stepper_Order);
    inline static constexpr auto s_error_order = static_cast<order_type>(Error_Order);

public:
    constexpr explicit_embedded_runge_kutta(rk_params_type rk_params) noexcept
        : m_rk_params{ rk_params }
    {
    }

    constexpr explicit_embedded_runge_kutta(
        size_type             n,
        rk_params_type const& rk_params
    ) noexcept
        : m_rk_params{ rk_params }
    {
        resize_internals(n);
    }

    [[nodiscard]]
    static constexpr auto stage_count() noexcept -> order_type
    {
        return s_stage_count;
    }

    [[nodiscard]]
    static constexpr auto error_stepper_order() noexcept -> order_type
    {
        return s_error_stepper_order;
    }

    [[nodiscard]]
    static constexpr auto error_order() noexcept -> order_type
    {
        return s_error_order;
    }

    auto do_step_impl(auto&& system, state_type& x_in_out, time_type& t) -> void
    {
        assert_size_compatibility(x_in_out.size());
        system(x_in_out, m_dxdt[0], t);

        while (true)
        {
            try_do_step_impl(std::forward<decltype(system)>(system), x_in_out, t, m_dt);
            m_dxdt_err = error_expr();
            m_dxdt_tmp = result_expr();
            m_dxdt_err /=
                (m_epsilon_abs +
                 m_epsilon_rel * (m_a_x * abs(x_in_out) + m_a_dxdt * abs(m_dxdt_tmp)));
            const auto val = data_types::operation_utils::linfinity_norm(m_dxdt_err);
            if (val > value_type(1))
            {
                m_dt *= std::max(
                    value_type(0.9) *
                        std::pow(val, value_type{ -1 } / value_type{ Stepper_Order - 1 }),
                    value_type(0.2)
                );
            }
            else if (val < value_type(0.5))
            {
                m_dt *= std::min(
                    value_type(0.9) *
                        std::pow(
                            val, value_type{ -1 } / value_type{ Error_Stepper_Order }
                        ),
                    value_type(5)
                );
            }
            else
            {
                break;
            }
        }
        x_in_out += m_dt * result_expr();
        t += m_dt;
    }

    auto try_do_step_impl(
        auto&&            system,
        state_type const& x_in_out,
        time_type         t,
        time_type         dt
    ) -> void
    {
        assert_size_compatibility(x_in_out.size());
        // F[0] will already be calculated
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
    }

    [[nodiscard]]
    constexpr auto result_expr() const noexcept -> auto const&
        requires data_types::dt_concepts::LazilyEvaluatedExpr<deriv_type, value_type>
    {
        static const auto expr = data_types::operation_utils::expr_reduce<Stage_Count>(
            m_dxdt, m_rk_params.b()
        );
        static_assert(data_types::dt_concepts::ExpressionTemplate<decltype(expr)>);
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

    [[nodiscard]]
    constexpr auto error_expr() const noexcept -> auto const&
        requires data_types::dt_concepts::LazilyEvaluatedExpr<deriv_type, value_type>
    {
        static const auto expr = data_types::operation_utils::expr_reduce<Stage_Count>(
            m_dxdt, m_rk_params.b_diff()
        );
        static_assert(data_types::dt_concepts::ExpressionTemplate<decltype(expr)>);
        return expr;
    }

    [[nodiscard]]
    constexpr auto error_expr() const noexcept -> auto
        requires data_types::dt_concepts::EagerlyEvaluatedExpr<deriv_type, value_type>
    {
        return data_types::operation_utils::expr_reduce<Stage_Count>(
            m_dxdt, m_rk_params.b_diff()
        );
    }

    auto assert_size_compatibility([[maybe_unused]] const size_type n) const noexcept
        -> void
    {
#ifndef NDEBUG
        if constexpr (data_types::dt_concepts::SizedInstance<state_type>)
        {
            assert(n == m_x_tmp.size());
        }
        if constexpr (data_types::dt_concepts::SizedInstance<deriv_type>)
        {
            assert(n == m_dxdt_err.size());
            assert(n == m_dxdt_tmp.size());
        }
        if constexpr (data_types::dt_concepts::SizedInstance<deriv_type> &&
                      data_types::dt_concepts::SizedInstance<state_type>)
        {
            for (auto const& dx : m_dxdt)
            {
                assert(n == dx.size());
            }
        }
#endif
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
        if constexpr (data_types::dt_concepts::Resizeable<state_type>)
        {
            m_dxdt_tmp.resize(n);
            m_dxdt_err.resize(n);
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
    rk_params_type m_rk_params;
    state_type     m_x_tmp;
    deriv_type     m_dxdt_err;
    deriv_type     m_dxdt_tmp;
    deriv_type     m_dxdt[Stage_Count];
    time_type      m_dt          = time_type(0.1);
    value_type     m_epsilon_abs = 1e-5;
    value_type     m_epsilon_rel = 1e-7;
    value_type     m_a_x         = 1;
    value_type     m_a_dxdt      = 1;
};

} // namespace solvers::explicit_stepers
