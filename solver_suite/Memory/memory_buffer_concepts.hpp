#pragma once

#include <concepts>

namespace buffers::mb_concepts
{

template <typename T>
concept VectorBufferShape = requires(T t) {
    typename T::size_type;
    T::s_dim;
    T::s_size;
    { t.size() } -> std::same_as<typename T::size_type>;
    { t.capacity() } -> std::same_as<typename T::size_type>;
    { t.dim() } -> std::same_as<typename T::size_type>;
    { t.flattened_size() } -> std::same_as<typename T::size_type>;
};

} // namespace buffers::mb_concepts
