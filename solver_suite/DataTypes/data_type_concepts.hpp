#pragma once

#include <iterator>

namespace data_types::dt_concepts
{

template <typename T>
concept StaticArray = requires(T t) {
    T::size;
    std::begin(t);
    std::end(t);
};

template <typename T>
concept DynamicArray = requires(T t) {
    t.size();
    std::begin(t);
    std::end(t);
};

template <typename T>
concept SizedInstance = requires(T t) { t.size(); };

} // namespace data_types::dt_concepts
