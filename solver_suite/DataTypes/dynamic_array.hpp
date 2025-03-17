#pragma once

#include "casts.hpp"
#include "data_type_concepts.hpp"
#include "data_type_utils.hpp"
#include "lazy_container_operations.hpp"
#include <concepts>
#include <initializer_list>
#include <iostream>
#include <memory>

#define DISABLE_MOVE

// #define DISABLE_COPY

namespace data_types::lazily_evaluated_containers
{

template <typename T, typename Allocator = std::allocator<T>>
class dynamic_array : private Allocator, dt_concepts::dynamic_array_base
{
public:
    using value_type  = T;
    using size_type   = std::size_t;
    using allocator_t = Allocator;

    using const_pointer  = value_type const*;
    using pointer        = value_type*;
    using const_iterator = const_pointer;
    using iterator       = pointer;

public:
    [[nodiscard]]
    constexpr auto allocator() const -> allocator_t const&
    {
        return *static_cast<allocator_t const*>(this);
    }

    [[nodiscard]]
    constexpr auto allocator() -> allocator_t&
    {
        return *static_cast<allocator_t*>(this);
    }

public:
    constexpr dynamic_array() noexcept
        : dynamic_array(allocator_t())
    {
    }

    explicit constexpr dynamic_array(allocator_t const& alloc) noexcept
        : allocator_t{ alloc }
        , begin_{ nullptr }
        , end_{ nullptr }
    {
    }

    explicit constexpr dynamic_array(
        size_type          size,
        allocator_t const& alloc = allocator_t()
    ) noexcept
        : allocator_t(alloc)
        , begin_{ allocator().allocate(size) }
        , end_{ begin_ + size }
    {
    }

    constexpr dynamic_array(
        size_type          size,
        value_type const&  value,
        allocator_t const& alloc = allocator_t()
    ) noexcept
        : allocator_t(alloc)
        , begin_{ allocator().allocate(size) }
        , end_{ begin_ + size }
    {
        std::fill(begin_, end_, value);
    }

    constexpr dynamic_array(
        std::initializer_list<T> init,
        const allocator_t        alloc = allocator_t()
    ) noexcept
        : allocator_t(alloc)
        , begin_{ allocator().allocate(init.size()) }
        , end_{ begin_ + init.size() }
    {
        std::ranges::copy(init, begin_);
    }

    constexpr dynamic_array(dt_concepts::ExpressionTemplate auto const& src) noexcept
        requires dt_concepts::SizedInstance<decltype(src)>
        : allocator_t()
        , begin_{ allocator().allocate(src.size()) }
        , end_{ begin_ + src.size() }
    {
        const auto n = src.size();
        for (size_type i = 0; i != n; ++i)
        {
            begin_[i] = src[i];
        }
    }

    constexpr auto operator=(dt_concepts::ExpressionTemplate auto const& src) noexcept
        -> dynamic_array&
    {
        const auto n = size();
        for (size_type i = 0; i != n; ++i)
        {
            begin_[i] = src[i];
        }
        return *this;
    }

#ifdef DISABLE_COPY
    constexpr dynamic_vector(dynamic_vector const&) noexcept                    = delete;
    constexpr auto operator=(dynamic_vector const&) noexcept -> dynamic_vector& = delete;
#else
    constexpr dynamic_array(dynamic_array const& other) noexcept
        : allocator_t(other.allocator())
        , begin_{ allocator().allocate(other.size()) }
        , end_{ begin_ + other.size() }
    {
        std::ranges::copy(other, this->begin_);
    }

    constexpr auto operator=(dynamic_array const& other) noexcept -> dynamic_array&
    {
        if (this != &other)
        {
            if (size() != other.size())
            {
                if (begin_)
                {
                    allocator().deallocate(begin_, size());
                    begin_ = nullptr;
                }
                allocator() = other.allocator();
                begin_      = allocator().allocate(other.size());
                end_        = begin_ + other.size();
            }
            std::ranges::copy(other, begin_);
        }
        return *this;
    }
#endif

#ifdef DISABLE_MOVE
    constexpr dynamic_array(dynamic_array&&) noexcept                    = delete;
    constexpr auto operator=(dynamic_array&&) noexcept -> dynamic_array& = delete;
#else
    constexpr dynamic_array(dynamic_array&& other) noexcept
        : begin_{ other.begin_ }
        , end_{ other.end_ }
    {
        other.begin_ = nullptr;
        other.end_   = nullptr;
    }

    constexpr auto operator=(dynamic_array&& other) noexcept -> dynamic_array&
    {
        if (this != &other)
        {
            if (begin_) allocator().deallocate(begin_, size());
            begin_       = other.begin_;
            end_         = other.end_;
            other.begin_ = nullptr;
            other.end_   = nullptr;
        }
        return *this;
    }
#endif

    ~dynamic_array() noexcept
    {
        if (begin_) allocator().deallocate(begin_, size());
    }

    constexpr auto leaky_resize(size_type n) & noexcept -> void
    {
        if (size() > n)
        {
            end_ = begin_ + n;
        }
        else
        {
            resize(n);
        }
    }

    constexpr auto resize(size_type n) & noexcept -> void
    {
        if (size() == n)
        {
            return;
        }
        if (begin_ != nullptr) [[unlikely]]
        {
            allocator().deallocate(begin_, size());
            begin_ = nullptr;
        }
        begin_ = allocator().allocate(n);
        end_   = begin_ + n;
    }

    [[nodiscard]]
    constexpr auto data(this auto&& self) noexcept -> decltype(auto)
    {
        return std::forward<decltype(self)>(self).begin_;
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

    constexpr auto operator+=(this auto& self, auto&& other) noexcept -> dynamic_array&
    {
        self.in_place_operator_impl_(std::forward<decltype(other)>(other), std::plus{});
        return self;
    }

    constexpr auto operator-=(this auto& self, auto&& other) noexcept -> dynamic_array&
    {
        self.in_place_operator_impl_(std::forward<decltype(other)>(other), std::minus{});
        return self;
    }

    constexpr auto operator*=(this auto& self, auto&& other) noexcept -> dynamic_array&
    {
        self.in_place_operator_impl_(
            std::forward<decltype(other)>(other), std::multiplies{}
        );
        return self;
    }

    constexpr auto operator/=(this auto& self, auto&& other) noexcept -> dynamic_array&
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
        assert(a.size() == dt_utils::common_size(a, std::forward<decltype(b)>(b)));
        for (auto idx = 0uz; idx != a.size(); ++idx)
        {
            a[idx] = std::invoke(
                std::forward<decltype(binary_op)>(binary_op),
                std::move(a[idx]),
                operation_utils::subscript(b, idx)
            );
        }
    }

    [[nodiscard]]
    constexpr auto operator<=>(dynamic_array const&) const = default;

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
auto operator<<(std::ostream& os, dynamic_array<T, Allocator> const& v) noexcept
    -> std::ostream&
{
    os << "{ ";
    const auto size = std::ranges::size(v);
    for (std::size_t n{ 0 }; auto const& e : v)
    {
        if constexpr (std::ranges::range<T>)
        {
            os << e << (++n != size ? ",\n" : "\n");
        }
        else
        {
            os << e << (++n != size ? ", " : " ");
        }
    }
    os << '}';

    return os;
}

} // namespace data_types::lazily_evaluated_containers

// std::common_type specialization
namespace std
{

template <typename T>
struct common_type<data_types::lazily_evaluated_containers::dynamic_array<T>, T>
{
    using type = data_types::lazily_evaluated_containers::dynamic_array<T>;
};

template <typename T>
struct common_type<T, data_types::lazily_evaluated_containers::dynamic_array<T>>
{
    using type = data_types::lazily_evaluated_containers::dynamic_array<T>;
};

} // namespace std
