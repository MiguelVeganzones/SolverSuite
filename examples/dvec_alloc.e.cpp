#define DEBUG_PRINT

#include "allocator_wrapper.hpp"
#include "debug_allocators.hpp"
#include "dynamic_array.hpp"
#include "stack_allocator.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello allocator world\n";

    using F              = float;
    using Allocator      = allocators::dynamic_stack_allocator<F>;
    using AllocatorPimpl = allocators::allocator_pimpl<Allocator>;
    using vector = data_types::lazily_evaluated_containers::dynamic_array<F, AllocatorPimpl>;
    Allocator allocator(100);
    static_assert(std::ranges::range<vector>);
    const auto n = 5;
    vector     v1(n, allocator);
    vector     v2(50, allocator);
    vector     v3(n, allocator);
    vector     v4(n, allocator);

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
        vector v5(5, allocator);
        v5 = v1 + v3;

        std::cout << v1 << '\n';
        std::cout << v3 << '\n';
        std::cout << v5 << '\n';
    }
    std::cout << "Goodbye allocator world\n";

    return 0;
}
