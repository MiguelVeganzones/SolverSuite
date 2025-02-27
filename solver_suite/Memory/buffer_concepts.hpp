#pragma once

#include <concepts>

namespace buffers::mb_concepts
{

template <typename T>
concept BufferShape = requires(T t) {
    typename T::size_type;
    T::s_size_y;
    T::s_size_x;
    { t.size_y() } -> std::same_as<typename T::size_type>;
    { t.size_x() } -> std::same_as<typename T::size_type>;
    { t.capacity_y() } -> std::same_as<typename T::size_type>;
    { t.capacity_x() } -> std::same_as<typename T::size_type>;
    { t.flat_size() } -> std::same_as<typename T::size_type>;
};

} // namespace buffers::mb_concepts
