#pragma once

#include <concepts>

namespace allocators::concepts
{

template <typename T>
concept BufferAllocator = requires(T t) {
    typename T::value_type;
    typename T::size_type;
    typename T::pointer;
    typename T::const_pointer;
    typename T::reference;
    typename T::const_reference;
    typename T::difference_type;
    { t.max_size() } -> std::same_as<typename T::size_type>;
    { t.used() } -> std::same_as<typename T::size_type>;
    { t.available() } -> std::same_as<typename T::size_type>;
    { t.reset() } -> std::same_as<void>;
    { t.allocate() } -> std::same_as<typename T::pointer>;
    { t.deallocate() } -> std::same_as<typename T::pointer>;
};

} // namespace allocators::concepts
