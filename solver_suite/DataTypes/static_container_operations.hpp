#pragma once

#include "operation_utils.hpp"
#include <functional>
#include <type_traits>
#include <utility>

namespace data_types::static_containers
{

constexpr auto operator+(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires requires { std::remove_reference_t<decltype(a)>::size; } ||
             requires { std::remove_reference_t<decltype(b)>::size; }

{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::plus{}
    );
}

constexpr auto operator-(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires requires { std::remove_reference_t<decltype(a)>::size; } ||
             requires { std::remove_reference_t<decltype(b)>::size; }
{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::minus{}
    );
}

constexpr auto operator*(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires requires { std::remove_reference_t<decltype(a)>::size; } ||
             requires { std::remove_reference_t<decltype(b)>::size; }
{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::multiplies{}
    );
}

constexpr auto operator/(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires requires { std::remove_reference_t<decltype(a)>::size; } ||
             requires { std::remove_reference_t<decltype(b)>::size; }
{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::divides{}
    );
}

[[nodiscard]]
constexpr auto operator_impl(auto&& a, auto&& b, auto&& binary_op) noexcept
    -> decltype(auto)
    requires requires { std::remove_reference_t<decltype(a)>::size; } ||
             requires { std::remove_reference_t<decltype(b)>::size; }
{
    using a_t   = std::remove_reference_t<decltype(a)>;
    using b_t   = std::remove_reference_t<decltype(b)>;
    using ret_t = std::common_type_t<a_t, b_t>;

    ret_t ret{};
    for (auto idx = 0uz; idx != std::ranges::size(ret); ++idx)
    {
        ret[idx] = binary_op(
            operation_utils::subscript(a, idx), operation_utils::subscript(b, idx)
        );
    }
    return ret;
}

} // namespace data_types::static_containers
