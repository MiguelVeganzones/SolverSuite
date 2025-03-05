#pragma once

#include <array>
#include <cassert>

#include <iostream>

namespace solvers::explicit_stepers
{

template <std::floating_point F, int Stage_Count>
struct runge_kutta_parameters
{
    using size_type                                                = int;
    inline static constexpr size_type                  stage_count = Stage_Count;
    std::array<F, (stage_count - 1) * stage_count / 2> params_a_;
    std::array<F, stage_count>                         params_b_;
    std::array<F, stage_count - 1>                     params_c_;

    [[nodiscard]]
    constexpr auto a() const noexcept -> auto const&
    {
        return params_a_;
    }

    [[nodiscard]]
    constexpr auto b() const noexcept -> auto const&
    {
        return params_b_;
    }

    [[nodiscard]]
    constexpr auto c() const noexcept -> auto const&
    {
        return params_c_;
    }

    [[nodiscard]]
    constexpr auto a(size_type j, size_type i) const noexcept -> auto const&
    {
        assert(j > 0 && j < stage_count);
        assert(i >= 0 && i < j);
        const auto idx = static_cast<std::size_t>(((j - 1) * j) / 2 + i);
        assert(idx < params_a_.size());
        return params_a_[idx];
    }

    [[nodiscard]]
    constexpr auto b(size_type i) const noexcept -> auto const&
    {
        assert(i >= 0 && i < stage_count);
        return params_b_[static_cast<std::size_t>(i)];
    }

    [[nodiscard]]
    constexpr auto c(size_type j) const noexcept -> auto const&
    {
        assert(j > 0 && j < stage_count);
        return params_c_[static_cast<std::size_t>(j - 1)];
    }
};

} // namespace solvers::explicit_stepers
