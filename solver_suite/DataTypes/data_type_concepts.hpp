#pragma once

#include "concepts.hpp"
#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

namespace data_types::dt_concepts
{

template <typename T>
concept SizedInstance = requires(T t) { t.size(); };

template <typename T>
concept Resizeable = requires(T t, const T ct, typename T::size_type n) {
    ct.size();
    t.resize(n);
};

template <typename T>
concept Indexable = requires(T t) {
    { t[std::declval<int>()] };
};

template <typename T>
concept Indexable2D = requires(T t) {
    { t[std::declval<int>(), std::declval<int>()] };
};

struct dynamic_array_base
{
};

struct data_buffer_base
{
};

struct expression_templates_base
{
};

template <typename T>
concept StaticArray =
    requires { T::s_size; } && Indexable<T> && SizedInstance<T> && std::ranges::range<T>;

template <typename T>
concept DynamicArray = std::is_base_of_v<dynamic_array_base, T> && Indexable<T> &&
                       SizedInstance<T> && Resizeable<T> && std::ranges::range<T>;

template <typename T>
concept Buffer = std::is_base_of_v<data_buffer_base, T> && Indexable<T> &&
                 Indexable2D<T> && SizedInstance<T> && std::ranges::range<T>;

template <typename T>
concept ExpressionTemplate =
    std::is_base_of_v<expression_templates_base, T> && Indexable<T>;

template <typename T>
concept ScalarType = utility::concepts::arithmetic<T>;

template <typename T>
concept ValidExprOperand = DynamicArray<T> || ExpressionTemplate<T> || ScalarType<T> ||
                           StaticArray<T> || Buffer<T>;

template <typename T>
concept EagerEvaluation = StaticArray<T> || ScalarType<T>;

template <typename T>
concept LazyEvaluation = ExpressionTemplate<T> || DynamicArray<T> || Buffer<T>;

template <typename A, typename B>
concept EagerlyEvaluatedExpr = EagerEvaluation<A> && EagerEvaluation<B>;

template <typename A, typename B>
concept LazilyEvaluatedExpr = LazyEvaluation<A> || LazyEvaluation<B>;

template <typename T>
concept ExprHoldReferenceType = DynamicArray<T> || Buffer<T>;

} // namespace data_types::dt_concepts
