#pragma once

#include "casts.hpp"
#include "static_container_operations.hpp"
#include <array>
#include <concepts>
#include <iostream>
#include <type_traits>

namespace data_types::static_containers
{

template <typename T, std::size_t N>
struct static_vector
{
    using value_type                  = T;
    using size_type                   = decltype(N);
    inline static constexpr auto size = N;
    using container_t                 = std::array<value_type, size>;
    using const_iterator              = typename container_t::const_iterator;
    using iterator                    = typename container_t::iterator;

    [[nodiscard]]
    constexpr auto data(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).data_;
    }

    [[nodiscard]]
    constexpr auto operator[](this auto&& self, std::integral auto idx) noexcept
        -> decltype(auto)
    {
        std::forward<decltype(self)>(self).assert_in_bounds(idx);
        return std::forward<decltype(self)>(self).data_[idx];
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

    constexpr auto operator+=(this auto& self, auto&& other) noexcept -> static_vector&
    {
        self = self + std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator-=(this auto& self, auto&& other) noexcept -> static_vector&
    {
        self = self - std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator*=(this auto& self, auto&& other) noexcept -> static_vector&
    {
        self = self * std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator/=(this auto& self, auto&& other) noexcept -> static_vector&
    {
        self = self / std::forward<decltype(other)>(other);
        return self;
    }

    [[nodiscard]]
    constexpr auto operator<=>(static_vector const&) const = default;

    inline constexpr auto assert_in_bounds([[maybe_unused]] std::integral auto const idx
    ) const noexcept -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(size));
    }

    container_t data_;
};

template <typename T, std::size_t N>
auto operator<<(std::ostream& os, static_vector<T, N> const& v) noexcept -> std::ostream&
{
    os << "{ ";
    for (std::size_t n{ 0 }; auto const e : v)
    {
        os << e << (++n != N ? ", " : " ");
    }
    os << '}';

    return os;
}

} // namespace data_types::static_containers

// std::common_type specialization
namespace std
{

template <typename T, std::size_t N>
struct common_type<data_types::static_containers::static_vector<T, N>, T>
{
    using type = data_types::static_containers::static_vector<T, N>;
};

template <typename T, std::size_t N>
struct common_type<T, data_types::static_containers::static_vector<T, N>>
{
    using type = data_types::static_containers::static_vector<T, N>;
};

} // namespace std
