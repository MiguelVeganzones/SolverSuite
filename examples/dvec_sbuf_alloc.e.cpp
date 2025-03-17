#define DEBUG_PRINT

#include "allocator_wrapper.hpp"
#include "debug_allocators.hpp"
#include "dynamic_array.hpp"
#include "random.hpp"
#include "stack_allocator.hpp"
#include "static_array.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello allocator world\n";

    using F              = float;
    constexpr auto N     = 15uz;
    const auto     n     = 6uz;
    using Allocator      = allocators::dynamic_stack_allocator<F>;
    using AllocatorPimpl = allocators::allocator_pimpl<Allocator>;
    using DVec     = data_types::lazily_evaluated_containers::dynamic_array<F, AllocatorPimpl>;
    using buffer_t = data_types::eagerly_evaluated_containers::static_array<DVec, N>;
    static_assert(std::ranges::range<buffer_t>);
    Allocator allocator(N * n);
    buffer_t  v1;
    buffer_t  v2;
    buffer_t  v3;
    buffer_t  v4;

    for (auto i = 0uz; i != N; ++i)
    {
        v1[i].allocator() = allocator;
        v1[i].resize(n);
        v2[i].allocator() = allocator;
        v2[i].resize(50);
        v3[i].allocator() = allocator;
        v3[i].resize(n);
        v4[i].allocator() = allocator;
        v4[i].resize(n);
    }

    std::cout << v1 << '\n';

    std::cout << &v1[0] << '\n';
    std::cout << &v2[0] << '\n';
    std::cout << &v3[0] << '\n';
    std::cout << &v4[0] << '\n';

    for (auto j = 0uz; j != N; ++j)
    {
        for (auto i = 0uz; i < n; ++i)
        {
            v1[j][i] = utility::random::srandom::randfloat<F>();
            v3[j][i] = utility::random::srandom::randfloat<F>();
        }
    }

    {
        buffer_t v5;
        for (auto& v : v5)
        {
            v.allocator() = allocator;
        }
        v5 = v1 + v3;
        // v5 += v3;

        std::cout << v5 << '\n';
    }
    std::cout << "Goodbye allocator world\n";

    return 0;
}
