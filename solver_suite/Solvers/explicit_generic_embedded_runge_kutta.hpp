#pragma once

#include "explicit_stepper_base.hpp"
#include <concepts>
#include <cstdint>
#include <utility>

namespace solvers::explicit_stepers
{

template <
    std::uint8_t        Primary_Stepper_Order,
    std::uint8_t        Secondary_Stepper_Order,
    std::uint8_t        Error_Order,
    std::floating_point Value_Type,
    typename State_Type,
    typename Deriv_Type,
    typename Time_Type>
class explicit_embedded_runge_kutta : explicit_stepers_base<
                                          explicit_embedded_runge_kutta<
                                              Primary_Stepper_Order,
                                              Secondary_Stepper_Order,
                                              Error_Order,
                                              Value_Type,
                                              State_Type,
                                              Deriv_Type,
                                              Time_Type>,
                                          Primary_Stepper_Order,
                                          Value_Type,
                                          State_Type,
                                          Deriv_Type,
                                          Time_Type>
{
public:
    using stepper_base_type = explicit_stepers_base<
        explicit_embedded_runge_kutta<
            Primary_Stepper_Order,
            Secondary_Stepper_Order,
            Error_Order,
            Value_Type,
            State_Type,
            Deriv_Type,
            Time_Type>,
        Primary_Stepper_Order,
        Value_Type,
        State_Type,
        Deriv_Type,
        Time_Type>;
    using size_type  = typename stepper_base_type::size_type;
    using order_type = typename stepper_base_type::order_type;
    using value_type = Value_Type;
    using state_type = State_Type;
    using deriv_type = Deriv_Type;
    using time_type  = Time_Type;

private:
    inline static constexpr auto s_primary_order =
        static_cast<order_type>(Primary_Stepper_Order);
    inline static constexpr auto s_secondary_order =
        static_cast<order_type>(Secondary_Stepper_Order);
    inline static constexpr auto s_error_order = static_cast<order_type>(Error_Order);

public:
    [[nodiscard]]
    static constexpr auto primary_order() noexcept -> order_type
    {
        return s_primary_order;
    }

    [[nodiscard]]
    static constexpr auto secondary_order() noexcept -> order_type
    {
        return s_secondary_order;
    }

    [[nodiscard]]
    static constexpr auto error_order() noexcept -> order_type
    {
        return s_error_order;
    }
};

} // namespace solvers::explicit_stepers
