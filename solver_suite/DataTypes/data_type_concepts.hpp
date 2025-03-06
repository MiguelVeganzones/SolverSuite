#pragma once

#include "concepts.hpp"
#include <iterator>
#include <utility>

namespace data_types::dt_concepts
{

template <typename T>
concept StaticArray = requires(T t) {
    T::size;
    std::begin(t);
    std::end(t);
};

template <typename T>
concept DynamicArray = requires(T t, const T ct, typename T::size_type n) {
    { ct.size() } -> std::same_as<typename T::size_type>;
    { t.resize(n) } -> std::same_as<void>;
    { std::begin(t) } -> std::input_or_output_iterator;
    { std::end(t) } -> std::input_or_output_iterator;
};

template <typename T>
concept SizedInstance = requires(T t) { t.size(); };

template <typename T>
concept Resizeable = requires(T t, typename T::size_type n) {
    t.size();
    t.resize(n);
};

struct expression_templates_base
{
};

template <typename T>
concept ExpressionTemplate = std::is_base_of_v<expression_templates_base, T>;

template <typename T>
concept ScalarType = utility::concepts::arithmetic<T>;

template <typename T>
concept ValidExprOperand = DynamicArray<T> || ExpressionTemplate<T> || ScalarType<T>;

} // namespace data_types::dt_concepts
