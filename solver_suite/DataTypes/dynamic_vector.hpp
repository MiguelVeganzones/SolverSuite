#pragma once

#include "casts.hpp"
#include "dynamic_container_operations.hpp"
#include <concepts>
#include <iostream>
#include <memory>
#include <span>
#include <type_traits>

#define DISABLE_MOVE_COPY

namespace data_types::dynamic_containers
{

template <typename T, typename Allocator = std::allocator<T>>
class dynamic_vector
{
public:
    using value_type  = T;
    using size_type   = std::size_t;
    using allocator_t = Allocator;

    using const_pointer  = value_type const*;
    using pointer        = value_type*;
    using const_iterator = const_pointer;
    using iterator       = pointer;

private:
    inline static allocator_t s_allocator = allocator_t(100);

public:
    constexpr dynamic_vector(size_type size)
        : begin_{ s_allocator.allocate(size) }
        , end_{ begin_ + size }
    {
    }

    constexpr auto operator=(expression_template auto const& src) noexcept
        -> dynamic_vector&
    {
        const auto n = size();
        for (size_type i = 0; i != n; ++i)
        {
            begin_[i] = src[i];
        }
        return *this;
    }

#ifdef DISABLE_MOVE_COPY
    constexpr dynamic_vector(dynamic_vector const&) noexcept                    = delete;
    constexpr dynamic_vector(dynamic_vector&&) noexcept                         = delete;
    constexpr auto operator=(dynamic_vector const&) noexcept -> dynamic_vector& = delete;
    constexpr auto operator=(dynamic_vector&&) noexcept -> dynamic_vector&      = delete;
#else
    constexpr dynamic_vector(dynamic_vector const& other) noexcept
        : begin_{ s_allocator.allocate(other.size()) }
        , end_{ begin_ + other.size() }
    {
        std::ranges::copy(other, this->begin_);
    }

    constexpr dynamic_vector(dynamic_vector&& other) noexcept
        : begin_(other.begin_)
        , end_(begin_ + other.size())
    {
        other.begin_ = nullptr;
        other.end_   = nullptr;
    }

    constexpr auto operator=(dynamic_vector const& other) noexcept -> dynamic_vector&
    {
        if (this != &other)
        {
            if (size() != other.size())
            {
                if (begin())
                {
                    s_allocator.deallocate(begin_, size());
                    begin_ = nullptr;
                }
                begin_ = s_allocator.allocate(other.size());
                end_   = begin_ + other.size();
            }
            std::ranges::copy(other, begin_);
        }
        return *this;
    }

    constexpr auto operator=(dynamic_vector&& other) noexcept -> dynamic_vector&
    {
        if (this != &other)
        {
            if (begin_) s_allocator.deallocate(begin_, size());
            begin_       = other.begin_;
            end_         = other.end_;
            other.begin_ = nullptr;
            other.end_   = nullptr;
        }
        return *this;
    }
#endif

    ~dynamic_vector() noexcept
    {
        if (begin_) s_allocator.deallocate(begin_, size());
    }

    [[nodiscard]]
    constexpr auto data(this auto&& self) noexcept -> decltype(auto)
    {
        return std::span(
            std::forward<decltype(self)>(self).begin_,
            std::forward<decltype(self)>(self).end_
        );
    }

    [[nodiscard]]
    constexpr auto operator[](this auto&& self, std::integral auto idx) noexcept
        -> decltype(auto)
    {
        std::forward<decltype(self)>(self).assert_in_bounds(idx);
        return std::forward<decltype(self)>(self).begin_[idx];
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> size_type
    {
        return static_cast<size_type>(std::distance(begin_, end_));
    }

    [[nodiscard]]
    constexpr auto cbegin() const noexcept -> const_iterator
    {
        return begin_;
    }

    [[nodiscard]]
    constexpr auto cend() const noexcept -> const_iterator
    {
        return end_;
    }

    [[nodiscard]]
    constexpr auto begin(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).begin_;
    }

    [[nodiscard]]
    constexpr auto end(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).end_;
    }

    constexpr auto operator+=(this auto& self, auto&& other) noexcept -> dynamic_vector&
    {
        self = self + std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator-=(this auto& self, auto&& other) noexcept -> dynamic_vector&
    {
        self = self - std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator*=(this auto& self, auto&& other) noexcept -> dynamic_vector&
    {
        self = self * std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator/=(this auto& self, auto&& other) noexcept -> dynamic_vector&
    {
        self = self / std::forward<decltype(other)>(other);
        return self;
    }

    [[nodiscard]]
    constexpr auto operator<=>(dynamic_vector const&) const = default;

private:
    inline constexpr auto assert_in_bounds([[maybe_unused]] std::integral auto const idx
    ) const noexcept -> void
    {
        assert(idx < utility::casts::safe_cast<decltype(idx)>(size()));
    }

private:
    pointer begin_;
    pointer end_;
};

template <typename T, typename Allocator>
auto operator<<(std::ostream& os, dynamic_vector<T, Allocator> const& v) noexcept
    -> std::ostream&
{
    os << "{ ";
    const auto size = std::ranges::size(v);
    for (std::size_t n{ 0 }; auto const e : v)
    {
        os << e << (++n != size ? ", " : " ");
    }
    os << '}';

    return os;
}

} // namespace data_types::dynamic_containers

// std::common_type specialization
namespace std
{

template <typename T>
struct common_type<data_types::dynamic_containers::dynamic_vector<T>, T>
{
    using type = data_types::dynamic_containers::dynamic_vector<T>;
};

template <typename T>
struct common_type<T, data_types::dynamic_containers::dynamic_vector<T>>
{
    using type = data_types::dynamic_containers::dynamic_vector<T>;
};

} // namespace std
