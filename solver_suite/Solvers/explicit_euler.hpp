#pragma once

#include "data_type_concepts.hpp"
#include "explicit_stepper_base.hpp"
#include <concepts>

namespace solvers::explicit_stepers
{

template <
    std::size_t         Stage_Count,
    std::size_t         Order,
    std::floating_point Value_Type,
    typename State_Type,
    typename Deriv_Type,
    typename Time_Type>
class explicit_euler : public explicit_stepers_base<
                           explicit_euler<
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
public:
    using size_type  = std::size_t;
    using value_type = Value_Type;
    using state_type = State_Type;
    using deriv_type = Deriv_Type;
    using time_type  = Time_Type;

private:
    inline static constexpr auto s_stage_count = Stage_Count;

public:
    [[nodiscard]]
    static constexpr auto stage_count() noexcept -> size_type
    {
        return s_stage_count;
    }

    auto do_step_impl(
        auto&&      system,
        state_type& x_in_out,
        time_type   t,
        time_type   dt
    ) noexcept -> void
    {
        if constexpr (data_types::dt_concepts::SizedInstance<deriv_type> &&
                      data_types::dt_concepts::SizedInstance<state_type>)
        {
            assert(x_in_out.size() == m_dxdt.size());
        }
        std::forward<decltype(system)>(system)(x_in_out, m_dxdt, t);
        x_in_out += m_dxdt * dt;
    }

    auto resize_internals(size_type n) noexcept -> void
    {
        if constexpr (data_types::dt_concepts::Resizeable<deriv_type>)
        {
            m_dxdt.resize(n);
        }
    }

private:
    deriv_type m_dxdt;
};

} // namespace solvers::explicit_stepers
