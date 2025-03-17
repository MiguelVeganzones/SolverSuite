#define DEBUG_PRINT

#include "allocator_wrapper.hpp"
#include "debug_allocators.hpp"
#include "dynamic_array.hpp"
#include "stack_allocator.hpp"
#include "static_array.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello allocator world\n";

    using F              = float;
    constexpr auto N     = 6uz;
    const auto     n     = 15;
    using SVec           = data_types::eagerly_evaluated_containers::static_array<F, N>;
    using Allocator      = allocators::dynamic_stack_allocator<SVec>;
    using AllocatorPimpl = allocators::allocator_pimpl<Allocator>;
    using vector = data_types::lazily_evaluated_containers::dynamic_array<SVec, AllocatorPimpl>;
    static_assert(std::ranges::range<vector>);
    Allocator allocator(N * n);
    vector    v1(n, allocator);
    vector    v2(50, allocator);
    vector    v3(n, allocator);
    vector    v4(n, allocator);

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
        vector v5(n, allocator);
        v5 = v1 + v3;

        std::cout << v5 << '\n';
    }
    std::cout << "Goodbye allocator world\n";

    return 0;
}
