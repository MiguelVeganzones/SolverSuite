#pragma once

#include "buffer_config.hpp"
#include "data_type_concepts.hpp"
#include "dynamic_array.hpp"
#include "lazy_container_operations.hpp"
#include "static_array.hpp"
#include <concepts>
#include <iostream>

namespace data_types::lazily_evaluated_containers
{

template <
    typename T,
    std::size_t                  Size_Y,
    std::size_t                  Size_X,
    buffer_config::LayoutPolicy  Layout_Policy,
    buffer_config::layout_stride Layout_Minor_Stride>
class static_buffer : dt_concepts::data_buffer_base
{
public:
    // TODO: signed integer types may be faster due to a narrower contract
    using value_type                                 = T;
    using size_type                                  = std::size_t;
    using index_t                                    = std::size_t;
    using layout_policy_t                            = buffer_config::LayoutPolicy;
    static constexpr size_type s_size_y              = Size_Y;
    static constexpr size_type s_size_x              = Size_X;
    static constexpr auto      s_layout_policy       = Layout_Policy;
    static constexpr auto      s_layout_minor_stride = Layout_Minor_Stride;
    static constexpr size_type s_underlying_size_y   = [] constexpr {
        if constexpr (s_layout_policy == layout_policy_t::layout_row_major)
        {
            return Size_Y;
        }
        else if constexpr (s_layout_policy == layout_policy_t::layout_column_major)
        {
            return Size_X;
        }
        else
        {
            static_assert(false, "Layout policy not recognized or not implemented");
        }
    }();
    static constexpr size_type s_underlying_size_x = [] constexpr {
        if constexpr (s_layout_policy == layout_policy_t::layout_row_major)
        {
            static_assert(
                !s_layout_minor_stride.padded() ||
                    Size_X <= s_layout_minor_stride.value(),
                "User defined stride must be greater than or equal to the corresponfing "
                "extent"
            );
            return std::max(Size_X, s_layout_minor_stride.value());
        }
        else if constexpr (s_layout_policy == layout_policy_t::layout_column_major)
        {
            static_assert(
                !s_layout_minor_stride.padded() ||
                    Size_Y <= s_layout_minor_stride.value(),
                "User defined stride must be greater than or equal to the corresponfing "
                "extent"
            );
            return std::max(Size_Y, s_layout_minor_stride.value());
        }
        else
        {
            static_assert(false, "Layout policy not recognized or not implemented");
        }
    }();
    static constexpr size_type s_underlying_size =
        s_underlying_size_x * s_underlying_size_y;
    using container_t = eagerly_evaluated_containers::static_array<T, s_underlying_size>;
    using const_iterator = typename container_t::const_iterator;
    using iterator       = typename container_t::iterator;

    static_assert(s_size_x > 0);
    static_assert(s_size_y > 0);

    constexpr auto operator=(dt_concepts::ExpressionTemplate auto const& src) noexcept
        -> static_buffer&
    {
        const auto n = size();
        for (size_type i = 0; i != n; ++i)
        {
            this->operator[](i) = src[i];
        }
        return *this;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto capacity_y() noexcept -> size_type
    {
        return size_y();
    }

    [[nodiscard, gnu::const]]
    static constexpr auto capacity_x() noexcept -> size_type
    {
        return size_x();
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size_y() noexcept -> size_type
    {
        return s_size_y;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size() noexcept -> size_type
    {
        return size_x() * size_y();
    }

    [[nodiscard, gnu::const]]
    static constexpr auto flat_size() noexcept -> size_type
    {
        return size_y() * size_x();
    }

    [[nodiscard, gnu::const]]
    static constexpr auto underlying_size_x() noexcept -> size_type
    {
        return s_underlying_size_x;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto underlying_size_y() noexcept -> size_type
    {
        return s_underlying_size_y;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto underlying_flat_size() noexcept -> size_type
    {
        return s_underlying_size;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto flat_projection(index_t idx_y, index_t idx_x) noexcept
        -> index_t
    {
        assert(idx_y < s_size_y);
        assert(idx_x < s_size_x);
        if constexpr (s_layout_policy == layout_policy_t::layout_column_major)
        {
            return idx_x * underlying_size_x() + idx_y;
        }
        else if constexpr (s_layout_policy == layout_policy_t::layout_row_major)
        {
            return idx_x + underlying_size_x() * idx_y;
        }
        else
        {
            static_assert(false, "Layout policy not recognized or not implemented");
        }
    }

    [[nodiscard]]
    constexpr auto data(this auto&& self) noexcept -> decltype(auto)
    {
        return std::begin(std::forward<decltype(self)>(self).data_);
    }

    [[nodiscard]]
    constexpr auto operator[](this auto&& self, std::integral auto idx) noexcept
        -> decltype(auto)
    {
        assert(0 <= idx && static_cast<size_type>(idx) < self.size());
        if constexpr (s_layout_minor_stride.padded())
        {
            const auto j = static_cast<index_t>(idx / size_x());
            const auto i = static_cast<index_t>(idx % size_x());
            idx          = flat_projection(j, i);
        }
        return std::forward<decltype(self)>(self).data_[idx];
    }

    [[nodiscard]]
    constexpr auto operator[](
        this auto&&        self,
        std::integral auto idx_y,
        std::integral auto idx_x
    ) noexcept -> decltype(auto)
    {
        assert(0 <= idx_y && static_cast<size_type>(idx_y) < self.size_y());
        assert(0 <= idx_x && static_cast<size_type>(idx_x) < self.size_x());
        return std::forward<decltype(self)>(self).data_[flat_projection(idx_y, idx_x)];
    }

    [[nodiscard]]
    constexpr auto cbegin() const noexcept -> const_iterator
        requires(!s_layout_minor_stride.padded())
    {
        return std::cbegin(data_);
    }

    [[nodiscard]]
    constexpr auto cend() const noexcept -> iterator
        requires(!s_layout_minor_stride.padded())
    {
        return std::cend(data_);
    }

    [[nodiscard]]
    constexpr auto begin(this auto&& self) noexcept -> decltype(auto)
        requires(!s_layout_minor_stride.padded())
    {
        return std::begin(std::forward<decltype(self)>(self).data_);
    }

    [[nodiscard]]
    constexpr auto end(this auto&& self) noexcept -> decltype(auto)
        requires(!s_layout_minor_stride.padded())
    {
        return std::end(std::forward<decltype(self)>(self).data_);
    }

    constexpr auto operator+=(this auto& self, auto&& other) noexcept -> static_buffer&
    {
        self.data_ += std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator-=(this auto& self, auto&& other) noexcept -> static_buffer&
    {
        self.data_ -= std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator*=(this auto& self, auto&& other) noexcept -> static_buffer&
    {
        self.data_ *= std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator/=(this auto& self, auto&& other) noexcept -> static_buffer&
    {
        self.data_ /= std::forward<decltype(other)>(other);
        return self;
    }

    [[nodiscard]]
    constexpr auto operator<=>(static_buffer const&) const = default;

    container_t data_;
};

template <typename T, std::size_t Size_X, typename Allocator = std::allocator<T>>
class dynamic_length_buffer
{
public:
    using value_type                    = T;
    using size_type                     = std::size_t;
    using index_t                       = std::size_t;
    using layout_policy_t               = buffer_config::LayoutPolicy;
    static constexpr size_type s_size_y = std::dynamic_extent;
    static constexpr size_type s_size_x = Size_X;
    static_assert(s_size_x > 0);

    constexpr dynamic_length_buffer(
        size_type                                length,
        size_type                                capacity,
        layout_policy_t                          layout_policy,
        data_types::buffer_config::layout_stride minor_stride
    ) noexcept
        : size_y_{ length }
        , capacity_y_{ std::max(capacity, size_y_) }
        , underlying_size_y_{ layout_policy == layout_policy_t::layout_row_major
                                  ? capacity_y_
                                  : s_size_x }
        , underlying_size_x_{ layout_policy == layout_policy_t::layout_row_major
                                  ? std::max(s_size_x, minor_stride.value())
                                  : std::max(capacity_y_, minor_stride.value()) }
        , stride_y_{ layout_policy == layout_policy_t::layout_row_major
                         ? underlying_size_x_
                         : size_type{ 1 } }
        , stride_x_{ layout_policy == layout_policy_t::layout_row_major
                         ? size_type{ 1 }
                         : underlying_size_x_ }
        , data_(underlying_size_y_ * underlying_size_x_)
        , layout_policy_{ layout_policy }
        , padded_{ minor_stride.padded() }
    {
        assert(
            layout_policy_ == layout_policy_t::layout_column_major ||
            layout_policy_ == layout_policy_t::layout_row_major
        );
        switch (layout_policy_)
        {
        case layout_policy_t::layout_column_major:
            assert(capacity_y_ <= minor_stride.value());
            break;
        case layout_policy_t::layout_row_major:
            assert(s_size_x <= minor_stride.value());
            break;
        default: utility::error_handling::assert_unreachable(); break;
        }
    }

    using container_t    = lazily_evaluated_containers::dynamic_array<T, Allocator>;
    using const_iterator = typename container_t::const_iterator;
    using iterator       = typename container_t::iterator;

public:
    [[nodiscard]]
    constexpr auto capacity_y() const noexcept -> size_type
    {
        return capacity_y_;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto capacity_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard]]
    constexpr auto size_y() const noexcept -> size_type
    {
        return size_y_;
    }

    [[nodiscard, gnu::const]]
    static constexpr auto size_x() noexcept -> size_type
    {
        return s_size_x;
    }

    [[nodiscard]]
    constexpr auto flat_size() const noexcept -> size_type
    {
        return size_y() * size_x();
    }

    [[nodiscard]]
    constexpr auto underlying_size_x() const noexcept -> size_type
    {
        return underlying_size_x_;
    }

    [[nodiscard]]
    constexpr auto underlying_size_y() const noexcept -> size_type
    {
        return underlying_size_y_;
    }

    [[nodiscard]]
    constexpr auto underlying_flat_size() const noexcept -> size_type
    {
        return underlying_size_y() * underlying_size_x();
    }

    [[nodiscard, gnu::const]]
    constexpr auto flat_projection(index_t idx_y, index_t idx_x) const noexcept -> index_t
    {
        assert(idx_y < size_y_);
        assert(idx_x < s_size_x);
        return idx_y * stride_y_ + idx_x * stride_x_;
    }

    [[nodiscard]]
    constexpr auto data(this auto&& self) noexcept -> decltype(auto)
    {
        return std::begin(std::forward<decltype(self)>(self).data_);
    }

    [[nodiscard]]
    constexpr auto operator[](this auto&& self, std::integral auto idx) noexcept
        -> decltype(auto)
    {
        assert(0 <= idx && static_cast<size_type>(idx) < self.size());
        if (self.padded_)
        {
            const auto j = static_cast<index_t>(idx / size_x());
            const auto i = static_cast<index_t>(idx % size_x());
            idx          = self.flat_projection(j, i);
        }
        return std::forward<decltype(self)>(self).data_[idx];
    }

    [[nodiscard]]
    constexpr auto operator[](
        this auto&&        self,
        std::integral auto idx_y,
        std::integral auto idx_x
    ) noexcept -> decltype(auto)
    {
        assert(0 <= idx_y && static_cast<size_type>(idx_y) < self.size_y());
        assert(0 <= idx_x && static_cast<size_type>(idx_x) < self.size_x());
        return std::forward<decltype(self)>(self)
            .data_[self.flat_projection(idx_y, idx_x)];
    }

    constexpr auto operator+=(this auto& self, auto&& other) noexcept
        -> dynamic_length_buffer&
    {
        self.data_ += std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator-=(this auto& self, auto&& other) noexcept
        -> dynamic_length_buffer&
    {
        self.data_ -= std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator*=(this auto& self, auto&& other) noexcept
        -> dynamic_length_buffer&
    {
        self.data_ *= std::forward<decltype(other)>(other);
        return self;
    }

    constexpr auto operator/=(this auto& self, auto&& other) noexcept
        -> dynamic_length_buffer&
    {
        self.data_ /= std::forward<decltype(other)>(other);
        return self;
    }

    [[nodiscard]]
    constexpr auto operator<=>(dynamic_length_buffer const&) const = default;

private:
    size_type       size_y_;
    size_type       capacity_y_;
    size_type       underlying_size_y_;
    size_type       underlying_size_x_;
    size_type       stride_y_;
    size_type       stride_x_;
    container_t     data_;
    layout_policy_t layout_policy_;
    bool            padded_;
};

template <
    typename T,
    std::size_t                  Size_Y,
    std::size_t                  Size_X,
    buffer_config::LayoutPolicy  Layout_Policy,
    buffer_config::layout_stride Layout_Minor_Stride>
auto operator<<(
    std::ostream&                                                               os,
    static_buffer<T, Size_Y, Size_X, Layout_Policy, Layout_Minor_Stride> const& v
) noexcept -> std::ostream&
{
    os << "{\n";
    for (auto j = 0; j != v.size_y(); ++j)
    {
        for (auto i = 0; i != v.size_x(); ++i)
        {
            os << ' ' << v[j, i] << (i != v.size_x() ? ',' : ' ');
        }
        os << '\n';
    }
    os << '}';

    return os;
}

} // namespace data_types::lazily_evaluated_containers
