#include "memory_buffers.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

int main()
{
    using namespace memory_buffers;
    constexpr auto N           = 7uz;
    constexpr auto D           = 5uz;
    constexpr auto size        = data_shape<N, D>{};
    constexpr auto layout      = LayoutPolicy::layout_sample_major_padded;
    constexpr auto layout_type = layout_info<layout, 8>{};
    auto           buffer      = static_stage_buffer<float, size, layout_type>{};

    std::ranges::fill(buffer.data_, std::numeric_limits<float>::quiet_NaN());

    for (auto const& e : buffer.data_)
    {
        std::cout << e << ' ';
    }
    std::cout << '\n';
    for (auto i = 0uz; i != N; ++i)
    {
        for (auto j = 0uz; j != D; ++j)
        {
            buffer[i, j] = float(i);
        }
    }
    for (auto i = 0uz; i != N; ++i)
    {
        for (auto j = 0uz; j != D; ++j)
        {
            std::cout << buffer[i, j] << ' ';
        }
        std::cout << '\n';
    }
    for (auto const& e : buffer.data_)
    {
        std::cout << e << ' ';
    }
}
