#define DEBUG_PRINT

#include "debug_allocators.hpp"
#include "dynamic_vector.hpp"
#include "random.hpp"
#include "stack_allocator.hpp"
#include "static_vector.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello allocator world\n";

    using F          = float;
    constexpr auto N = 15uz;
    const auto     n = 6uz;
    using Allocator  = allocators::dynamic_stack_allocator<F>;
    using DVec       = data_types::dynamic_containers::dynamic_vector<F, Allocator>;
    using buffer_t   = data_types::static_containers::static_vector<DVec, N>;
    static_assert(std::ranges::range<buffer_t>);
    Allocator allocator(N * n);
    DVec::set_allocator(&allocator);
    const auto m = data_types::static_containers::initialize_internals_placeholder{};
    buffer_t   v1(m, n);
    buffer_t   v2(m, 50);
    buffer_t   v3(m, n);
    buffer_t   v4(m, n);

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
        // This are the semantics unfortunately. Size is not know through the
        // expression template. This could be added, but copy and move
        // operations defeat the purpose of the stack allocator.
        buffer_t v5 = v1;
        v5 += v3;

        std::cout << v5 << '\n';
    }
    std::cout << "Goodbye allocator world\n";

    return 0;
}
