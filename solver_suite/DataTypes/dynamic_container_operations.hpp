#pragma once

#include "data_type_utils.hpp"
#include "operation_utils.hpp"
#include <functional>
#include <tuple>

// Needs to be in the same namespace for name resolution of constructors,
// unfortunately
namespace data_types::dynamic_containers
{

template <typename T>
concept expression_template = requires(T t) {
    T::expression_template_disambiguator;
    t.size();
};

template <typename Callable, typename... Operands>
class expr
{
public:
    inline static constexpr auto expression_template_disambiguator = 1;
    using callable_t                                               = Callable;
    using size_type                                                = std::size_t;

public:
    constexpr expr(size_type size, callable_t f, Operands const&... args)
        : m_args(args...)
        , m_f{ f }
        , m_size{ size }
    {
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> size_type
    {
        return m_size;
    }

    [[nodiscard]]
    inline constexpr auto operator[](std::integral auto idx) const
    {
        assert(idx < m_size);
        const auto apply_at_index =
            [this, idx] [[nodiscard]]
            (Operands const&... args) constexpr noexcept {
                return m_f(operation_utils::subscript(args, idx)...);
            };
        return std::apply(apply_at_index, m_args);
    }

private:
    std::tuple<Operands const&...> m_args;
    callable_t                     m_f;
    size_type                      m_size;
};

auto operator+(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto&& l, auto&& r) noexcept {
                     return std::invoke(
                         std::plus{},
                         std::forward<decltype(l)>(l),
                         std::forward<decltype(r)>(r)
                     );
                 },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator-(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto&& l, auto&& r) noexcept {
                     return std::invoke(
                         std::minus{},
                         std::forward<decltype(l)>(l),
                         std::forward<decltype(r)>(r)
                     );
                 },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator*(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto&& l, auto&& r) noexcept {
                     return std::invoke(
                         std::multiplies{},
                         std::forward<decltype(l)>(l),
                         std::forward<decltype(r)>(r)
                     );
                 },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

auto operator/(auto&& lhs, auto&& rhs) noexcept -> decltype(auto)
{
    const auto size = dt_utils::common_size(lhs, rhs);
    return expr{ size,
                 [](auto&& l, auto&& r) noexcept {
                     return std::invoke(
                         std::divides{},
                         std::forward<decltype(l)>(l),
                         std::forward<decltype(r)>(r)
                     );
                 },
                 std::forward<decltype(lhs)>(lhs),
                 std::forward<decltype(rhs)>(rhs) };
}

} // namespace data_types::dynamic_containers

//
//
/*
  //
  // class _Expr
  //
  template<class _Clos, typename _Tp>
    class _Expr
    {
    public:
      typedef _Tp value_type;

      _Expr(const _Clos&);

      const _Clos& operator()() const;

      value_type operator[](size_t) const;
      valarray<value_type> operator[](slice) const;
      valarray<value_type> operator[](const gslice&) const;
      valarray<value_type> operator[](const valarray<bool>&) const;
      valarray<value_type> operator[](const valarray<size_t>&) const;

      _Expr<_UnClos<__unary_plus, std::_Expr, _Clos>, value_type>
      operator+() const;

      _Expr<_UnClos<__negate, std::_Expr, _Clos>, value_type>
      operator-() const;

      _Expr<_UnClos<__bitwise_not, std::_Expr, _Clos>, value_type>
      operator~() const;

      _Expr<_UnClos<__logical_not, std::_Expr, _Clos>, bool>
      operator!() const;

      size_t size() const;
      value_type sum() const;

      valarray<value_type> shift(int) const;
      valarray<value_type> cshift(int) const;

      value_type min() const;
      value_type max() const;

      valarray<value_type> apply(value_type (*)(const value_type&)) const;
      valarray<value_type> apply(value_type (*)(value_type)) const;

    private:
      const _Clos _M_closure;
    };

  template<class _Clos, typename _Tp>
    inline
    _Expr<_Clos, _Tp>::_Expr(const _Clos& __c) : _M_closure(__c) {}

  template<class _Clos, typename _Tp>
    inline const _Clos&
    _Expr<_Clos, _Tp>::operator()() const
    { return _M_closure; }

  template<class _Clos, typename _Tp>
    inline _Tp
    _Expr<_Clos, _Tp>::operator[](size_t __i) const
    { return _M_closure[__i]; }

  template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos, _Tp>::operator[](slice __s) const
    {
      valarray<_Tp> __v = valarray<_Tp>(*this)[__s];
      return __v;
    }

  template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos, _Tp>::operator[](const gslice& __gs) const
    {
      valarray<_Tp> __v = valarray<_Tp>(*this)[__gs];
      return __v;
    }

  template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos, _Tp>::operator[](const valarray<bool>& __m) const
    {
      valarray<_Tp> __v = valarray<_Tp>(*this)[__m];
      return __v;
    }

  template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos, _Tp>::operator[](const valarray<size_t>& __i) const
    {
      valarray<_Tp> __v = valarray<_Tp>(*this)[__i];
      return __v;
    }

  template<class _Clos, typename _Tp>
    inline size_t
    _Expr<_Clos, _Tp>::size() const
    { return _M_closure.size(); }

  template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos, _Tp>::shift(int __n) const
    {
      valarray<_Tp> __v = valarray<_Tp>(*this).shift(__n);
      return __v;
    }
*/
