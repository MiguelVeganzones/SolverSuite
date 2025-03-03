#pragma once

namespace data_types::dt_concepts
{

template <typename T>
concept StaticArray = requires(T t) { T::size; };

} // namespace data_types::dt_concepts
