#include "random.hpp"
#define DEBUG_PRINT

#include "debug_allocators.hpp"
#include "stack_allocator.hpp"
#include "static_array.hpp"
#include <iostream>

// Non defualt constructible class
class ndc
{
public:
    ndc(float f)
        : _f(f) {};

    float value() const
    {
        return _f;
    }

private:
    float _f;
};

auto operator<<(std::ostream& os, ndc const& v) noexcept -> std::ostream&
{
    os << v.value();
    return os;
}

int main()
{
    std::cout << "Hello global allocator world\n";

    using F          = float;
    constexpr auto N = 10uz;
    using vector_f   = data_types::eagerly_evaluated_containers::static_array<F, N>;
    using vector_ndc = data_types::eagerly_evaluated_containers::static_array<ndc, N>;
    vector_f v1;
    vector_f v2;
    vector_f v3;
    // vector_ndc v4; // Invalid, no default ctor
    vector_ndc v6 = vector_ndc::filled(utility::random::srandom::randfloat<F>);

    std::cout << v1 << '\n';
    std::cout << v6 << '\n';

    std::cout << &v1[0] << '\n';
    std::cout << &v2[0] << '\n';
    std::cout << &v3[0] << '\n';

    for (auto i = 0; i != N; ++i)
    {
        v1[i] = float(i);
        v3[i] = float(i * i);
    }

    {
        // This are the semantics unfortunately. Size is not know through the
        // expression template. This could be added, but copy and move
        // operations defeat the purpose of the stack allocator.
        vector_f v5 = v1 + v3;

        std::cout << v1 << '\n';
        std::cout << v3 << '\n';
        std::cout << v5 << '\n';
    }

    std::cout << "d: " << std::distance((int*)(0), (int*)(0)) << '\n';

    std::cout << "Goodbye global allocator world\n";

    return 0;
}
