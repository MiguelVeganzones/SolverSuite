#pragma once

#include <array>
#include <cassert>

namespace solvers::explicit_stepers
{

template <std::floating_point F, int Stage_Count>
struct butcher_tableau
{
    using size_type                               = int;
    using value_type                              = F;
    inline static constexpr size_type stage_count = Stage_Count;

    using params_a_type = std::array<F, (stage_count - 1) * stage_count / 2>;
    using params_b_type = std::array<F, stage_count>;
    using params_c_type = std::array<F, stage_count - 1>;

    params_a_type params_a_;
    params_b_type params_b_;
    params_c_type params_c_;

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

template <std::floating_point F, int Stage_Count>
class extended_butcher_tableau
{
public:
    using size_type                               = int;
    using value_type                              = F;
    inline static constexpr size_type stage_count = Stage_Count;

    using butcher_tableau_t = butcher_tableau<F, Stage_Count>;
    using params_a_type     = typename butcher_tableau_t::params_a_type;
    using params_b_type     = typename butcher_tableau_t::params_b_type;
    using params_c_type     = typename butcher_tableau_t::params_c_type;

    extended_butcher_tableau(
        params_a_type a,
        params_b_type b,
        params_b_type b_err,
        params_c_type c
    )
        : m_rk_params(a, b, c)
    {
        for (auto i = size_type{}; auto& e : m_b_diff)
        {
            e = b[i] - b_err[i];
            ++i;
        }
    }

    [[nodiscard]]
    constexpr auto a() const noexcept -> auto const&
    {
        return m_rk_params.a();
    }

    [[nodiscard]]
    constexpr auto b() const noexcept -> auto const&
    {
        return m_rk_params.b();
    }

    [[nodiscard]]
    constexpr auto b_diff() const noexcept -> auto const&
    {
        return m_b_diff;
    }

    [[nodiscard]]
    constexpr auto c() const noexcept -> auto const&
    {
        return m_rk_params.c();
    }

    [[nodiscard]]
    constexpr auto a(size_type j, size_type i) const noexcept -> auto const&
    {
        return m_rk_params.a(j, i);
    }

    [[nodiscard]]
    constexpr auto b(size_type i) const noexcept -> auto const&
    {
        return m_rk_params.b(i);
    }

    [[nodiscard]]
    constexpr auto b_diff(size_type i) const noexcept -> auto const&
    {
        assert(i >= 0 && i < stage_count);
        return m_b_diff[i];
    }

    [[nodiscard]]
    constexpr auto c(size_type j) const noexcept -> auto const&
    {
        return m_rk_params.c(j);
    }

    butcher_tableau<value_type, Stage_Count> m_rk_params;
    params_b_type                            m_b_diff;
};

} // namespace solvers::explicit_stepers
