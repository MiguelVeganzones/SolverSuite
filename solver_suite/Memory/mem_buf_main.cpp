#include "buffer_config.hpp"
#include "buffer_interface.hpp"
#include "debug_allocators.hpp"
#include "monotonic_allocator.hpp"
#include "uniform_monotonic_multipool_allocator.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

int main()
{
    using namespace buffers;
    using F                     = float;
    constexpr auto N            = 7uz;
    constexpr auto D            = 5uz;
    constexpr auto layout       = buffer_config::LayoutPolicy::layout_row_major;
    constexpr auto minor_stride = 8uz;
    // constexpr auto interface = buffer_interface::static_shape<N, D, layout, 8>{};
    // const auto interface = buffer_interface::dynamic_length<D>(N, 10, layout,
    // minor_stride);
    const auto interface =
        buffer_interface::dynamic_shape(N, 10, D, D, layout, minor_stride);

    std::cout << interface.underlying_size_y() << '\n';
    std::cout << interface.underlying_size_x() << '\n';

    auto allocator = allocators::dynamic_uniform_monotonic_multipool_allocator<F, 5>(
        interface.underlying_flat_size()
    );
    const auto n   = interface.underlying_flat_size();
    auto       ptr = allocator.allocate(n);

    std::span buffer(ptr, n);
    std::ranges::fill(buffer, std::numeric_limits<float>::quiet_NaN());

    std::cout << '\n';
    for (auto i = 0uz; i != N; ++i)
    {
        for (auto j = 0uz; j != D; ++j)
        {
            buffer[interface.translate_idx(i, j)] = float(i);
        }
    }
    for (auto i = 0uz; i != N; ++i)
    {
        for (auto j = 0uz; j != D; ++j)
        {
            std::cout << buffer[interface.translate_idx(i, j)] << ' ';
        }
        std::cout << '\n';
    }
    for (auto const& e : buffer)
    {
        std::cout << e << ' ';
    }
}
