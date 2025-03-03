#define DEBUG_PRINT

#include "debug_allocators.hpp"
#include "dynamic_vector.hpp"
#include "stack_allocator.hpp"
#include "static_vector.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello allocator world\n";

    using F          = float;
    constexpr auto N = 6uz;
    const auto     n = 15;
    using SVec       = data_types::static_containers::static_vector<F, N>;
    using Allocator  = allocators::dynamic_stack_allocator<SVec>;
    using vector     = data_types::dynamic_containers::dynamic_vector<SVec, Allocator>;
    static_assert(std::ranges::range<vector>);
    Allocator allocator(N * n);
    vector::set_allocator(&allocator);
    vector v1(n);
    vector v2(50);
    vector v3(n);
    vector v4(n);

    std::cout << v1 << '\n';

    std::cout << &v1[0] << '\n';
    std::cout << &v2[0] << '\n';
    std::cout << &v3[0] << '\n';
    std::cout << &v4[0] << '\n';

    for (auto i = 0.f; i < float(n); ++i)
    {
        // This works since size is a compile time constant and there is a
        // std::initializer list constructor (unfortunately)
        v1[(int)i] = { i, i, i, i, i, i };
        // This is a better way to fill the arrays
        std::ranges::fill(v3[(int)i], i);
    }

    {
        // This are the semantics unfortunately. Size is not know through the
        // expression template. This could be added, but copy and move
        // operations defeat the purpose of the stack allocator.
        vector v5(n);
        v5 = v1 + v3;

        std::cout << v5 << '\n';
    }
    std::cout << "Goodbye allocator world\n";

    return 0;
}
