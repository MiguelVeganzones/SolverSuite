#pragma once

#include "casts.hpp"
#include "compile_time_utility.hpp"
#include "data_type_concepts.hpp"
#include "static_container_operations.hpp"
#include <array>
#include <concepts>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <type_traits>

namespace data_types::static_containers
{

template <typename T, std::size_t N>
struct static_array
{
    using value_type                    = T;
    using size_type                     = decltype(N);
    inline static constexpr auto s_size = N;
    using container_t                   = std::array<value_type, s_size>;
    using const_iterator                = typename container_t::const_iterator;
    using iterator                      = typename container_t::iterator;

    static constexpr auto filled(auto&&... args) noexcept -> static_array
        requires std::constructible_from<T, decltype(args)...>
    {
        return static_array{
            utility::compile_time_utility::array_factory<value_type, s_size>(
                value_type(std::forward<decltype(args)>(args)...)
            )
        };
    }

    static constexpr auto filled(auto&& fn, auto&&... args) noexcept -> static_array
        requires std::is_invocable_v<decltype(fn), decltype(args)...> &&
                 std::constructible_from<
                     T,
                     std::invoke_result_t<decltype(fn), decltype(args)...>>
    {
        return static_array{
            utility::compile_time_utility::array_factory<value_type, s_size>(
                std::forward<decltype(fn)>(fn), std::forward<decltype(args)>(args)...
            )
        };
    }

    [[nodiscard]]
    static constexpr auto size() noexcept -> size_type
    {
        return s_size;
    }

    [[nodiscard]]
    constexpr auto data(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).data_.data();
    }

    [[nodiscard]]
    constexpr auto operator[](this auto&& self, std::integral auto idx) noexcept
        -> decltype(auto)
    {
        std::forward<decltype(self)>(self).assert_in_bounds(idx);
        return std::forward<decltype(self)>(self).data_[idx];
    }

    template <typename Slice_Type = std::span<value_type>>
    [[nodiscard]]
    constexpr auto slice(this auto&& self, std::size_t start, std::size_t end) noexcept
        -> decltype(auto)
    {
        assert(start <= end);
        assert(end <= s_size);
        if constexpr (std::is_same_v<Slice_Type, std::span<value_type>>)
        {
            return Slice_Type{ self.begin_ + start, self.begin_ + end };
        }
        else if constexpr (dt_concepts::StaticArray<Slice_Type>)
        {
            Slice_Type ret{};
            std::copy(self.begin() + start, self.begin() + end, std::begin(ret));
            return ret;
        }
        else
        {
            Slice_Type ret(s_size);
            std::copy(self.begin() + start, self.begin() + end, std::begin(ret));
            return ret;
        }
    }

    [[nodiscard]]
    constexpr auto cbegin() const noexcept -> container_t::const_iterator
    {
        return std::cbegin(data_);
    }

    [[nodiscard]]
    constexpr auto cend() const noexcept -> container_t::const_iterator
    {
        return std::cend(data_);
    }

    [[nodiscard]]
    constexpr auto begin(this auto&& self) noexcept -> decltype(auto)
    {
        return std::begin(std::forward<decltype(self)>(self).data_);
    }

    [[nodiscard]]
    constexpr auto end(this auto&& self) noexcept -> decltype(auto)
    {
        return std::end(std::forward<decltype(self)>(self).data_);
    }

    constexpr auto operator+=(this auto& self, auto&& other) noexcept -> static_array&
    {
        self.in_place_operator_impl_(std::forward<decltype(other)>(other), std::plus{});
        return self;
    }

    constexpr auto operator-=(this auto& self, auto&& other) noexcept -> static_array&
    {
        self.in_place_operator_impl_(std::forward<decltype(other)>(other), std::minus{});
        return self;
    }

    constexpr auto operator*=(this auto& self, auto&& other) noexcept -> static_array&
    {
        self.in_place_operator_impl_(
            std::forward<decltype(other)>(other), std::multiplies{}
        );
        return self;
    }

    constexpr auto operator/=(this auto& self, auto&& other) noexcept -> static_array&
    {
        self.in_place_operator_impl_(
            std::forward<decltype(other)>(other), std::divides{}
        );
        return self;
    }

    constexpr auto in_place_operator_impl_(
        this auto& a,
        auto&&     b,
        auto&&     binary_op
    ) noexcept -> void
    {
        using a_t = std::remove_reference_t<decltype(a)>;
        using b_t = std::remove_reference_t<decltype(b)>;

        if constexpr (dt_concepts::StaticArray<a_t> && dt_concepts::StaticArray<b_t>)
        {
            static_assert(dt_traits::is_same_size_v<a_t, b_t>);
        }

        for (auto idx = 0uz; idx != s_size; ++idx)
        {
            a[idx] = binary_op(a[idx], operation_utils::subscript(b, idx));
        }
    }

    [[nodiscard]]
    constexpr auto operator<=>(static_array const&) const = default;

    inline constexpr auto assert_in_bounds([[maybe_unused]] std::integral auto idx
    ) const noexcept -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(s_size));
    }

    container_t data_;
};

template <typename T, std::size_t N>
auto operator<<(std::ostream& os, static_array<T, N> const& v) noexcept -> std::ostream&
{
    os << "{ ";
    for (std::size_t n{ 0 }; auto const& e : v)
    {
        if constexpr (std::ranges::range<T>)
        {
            os << e << (++n != N ? ",\n" : "\n");
        }
        else
        {
            os << e << (++n != N ? ", " : " ");
        }
    }
    os << '}';

    return os;
}

} // namespace data_types::static_containers

// std::common_type specialization
namespace std

{

template <typename T, std::size_t N>
struct common_type<data_types::static_containers::static_array<T, N>, T>
{
    using type = data_types::static_containers::static_array<T, N>;
};

template <typename T, std::size_t N>
struct common_type<T, data_types::static_containers::static_array<T, N>>
{
    using type = data_types::static_containers::static_array<T, N>;
};

} // namespace std
