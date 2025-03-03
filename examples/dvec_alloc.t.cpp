#define DEBUG_PRINT

#include "debug_allocators.hpp"
#include "dynamic_vector.hpp"
#include "stack_allocator.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello allocator world\n";

    using F         = float;
    using Allocator = allocators::dynamic_stack_allocator<F>;
    using vector    = data_types::dynamic_containers::dynamic_vector<F, Allocator>;
    Allocator allocator(100);
    vector::set_allocator(&allocator);
    static_assert(std::ranges::range<vector>);
    const auto n = 5;
    vector     v1(n);
    vector     v2(50);
    vector     v3(n);
    vector     v4(n);

    std::cout << v1 << '\n';

    std::cout << &v1[0] << '\n';
    std::cout << &v2[0] << '\n';
    std::cout << &v3[0] << '\n';
    std::cout << &v4[0] << '\n';

    for (auto i = 0; i != n; ++i)
    {
        v1[i] = float(i);
        v3[i] = float(i * i);
    }

    {
        // This are the semantics unfortunately. Size is not know through the
        // expression template. This could be added, but copy and move
        // operations defeat the purpose of the stack allocator.
        vector v5(5);
        v5 = v1 + v3;

        std::cout << v1 << '\n';
        std::cout << v3 << '\n';
        std::cout << v5 << '\n';
    }
    std::cout << "Goodbye allocator world\n";

    return 0;
}
