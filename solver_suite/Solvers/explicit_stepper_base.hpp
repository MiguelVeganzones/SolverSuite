#pragma once

#include <concepts>
#include <utility>

namespace solvers::explicit_stepers
{

template <
    typename Stepper,
    std::size_t         Order,
    std::floating_point Value_Type,
    typename State_Type,
    typename Deriv_Type,
    typename Time_Type>
class explicit_stepers_base
{
public:
    using stepper_type = Stepper;
    using size_type    = std::size_t;
    using value_type   = Value_Type;
    using state_type   = State_Type;
    using deriv_type   = Deriv_Type;
    using time_type    = Time_Type;

private:
    inline static constexpr auto s_order = Order;

public:
    [[nodiscard]]
    constexpr auto stepper() noexcept -> stepper_type&
    {
        return *static_cast<stepper_type*>(this);
    }

    [[nodiscard]]
    constexpr auto stepper() const noexcept -> stepper_type const&
    {
        return *static_cast<stepper_type const*>(this);
    }

    [[nodiscard]]
    static constexpr auto order() noexcept -> size_type
    {
        return s_order;
    }

    auto do_step(auto&& system, state_type& x_in_out, time_type t, time_type dt) noexcept
        -> void
    {
        this->stepper().do_step_impl(
            std::forward<decltype(system)>(system), x_in_out, t, dt
        );
    }
};

} // namespace solvers::explicit_stepers
