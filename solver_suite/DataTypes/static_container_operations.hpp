#pragma once

#include "data_type_concepts.hpp"
#include "data_type_traits.hpp"
#include "operation_utils.hpp"
#include <functional>
#include <type_traits>
#include <utility>

namespace data_types::static_containers
{

constexpr auto operator+(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires dt_concepts::StaticArray<std::remove_reference_t<decltype(a)>> ||
             dt_concepts::StaticArray<std::remove_reference_t<decltype(b)>>

{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::plus{}
    );
}

constexpr auto operator-(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires dt_concepts::StaticArray<std::remove_reference_t<decltype(a)>> ||
             dt_concepts::StaticArray<std::remove_reference_t<decltype(b)>>
{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::minus{}
    );
}

constexpr auto operator*(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires dt_concepts::StaticArray<std::remove_reference_t<decltype(a)>> ||
             dt_concepts::StaticArray<std::remove_reference_t<decltype(b)>>
{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::multiplies{}
    );
}

constexpr auto operator/(auto&& a, auto&& b) noexcept -> decltype(auto)
    requires dt_concepts::StaticArray<std::remove_reference_t<decltype(a)>> ||
             dt_concepts::StaticArray<std::remove_reference_t<decltype(b)>>
{
    return operator_impl(
        std::forward<decltype(a)>(a), std::forward<decltype(b)>(b), std::divides{}
    );
}

[[nodiscard]]
constexpr auto operator_impl(auto&& a, auto&& b, auto&& binary_op) noexcept
    -> decltype(auto)
    requires dt_concepts::StaticArray<std::remove_reference_t<decltype(a)>> ||
             dt_concepts::StaticArray<std::remove_reference_t<decltype(b)>>
{
    using a_t   = std::remove_reference_t<decltype(a)>;
    using b_t   = std::remove_reference_t<decltype(b)>;
    using ret_t = std::common_type_t<a_t, b_t>;

    if constexpr (dt_concepts::StaticArray<a_t> && dt_concepts::StaticArray<b_t>)
    {
        static_assert(dt_traits::is_same_size_v<a_t, b_t>);
    }

    if constexpr (dt_concepts::StaticArray<a_t>)
    {
        ret_t ret(a);
        for (auto i = 0uz; i != std::ranges::size(a); ++i)
        {
            ret[i] = std::invoke(
                binary_op,
                operation_utils::subscript(a, i),
                operation_utils::subscript(b, i)
            );
        }
        return ret;
    }
    else if constexpr (dt_concepts::StaticArray<b_t>)
    {
        ret_t ret(b);
        for (auto i = 0uz; i != std::ranges::size(a); ++i)
        {
            ret[i] = std::invoke(
                binary_op,
                operation_utils::subscript(a, i),
                operation_utils::subscript(b, i)
            );
        }
        return ret;
    }
    else
    {
        static_assert(false, "Unreachable code");
    }
}

} // namespace data_types::static_containers
