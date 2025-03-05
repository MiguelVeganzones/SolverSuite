#pragma once

#include "data_type_concepts.hpp"
#include "explicit_stepper_base.hpp"
#include "runge_kutta_params.hpp"
#include <cassert>
#include <concepts>
#include <limits>
#include <numeric>

namespace solvers::explicit_stepers
{

template <
    std::size_t         Stage_Count,
    std::size_t         Order,
    std::floating_point Value_Type,
    typename State_Type,
    typename Deriv_Type,
    typename Time_Type>
class generic_runge_kutta_base : public explicit_stepers_base<
                                     generic_runge_kutta_base<
                                         Stage_Count,
                                         Order,
                                         Value_Type,
                                         State_Type,
                                         Deriv_Type,
                                         Time_Type>,
                                     Order,
                                     Value_Type,
                                     State_Type,
                                     Deriv_Type,
                                     Time_Type>
{
private:
    inline static constexpr auto s_stage_count = (int)Stage_Count;

public:
    using size_type      = std::size_t;
    using value_type     = Value_Type;
    using state_type     = State_Type;
    using deriv_type     = Deriv_Type;
    using time_type      = Time_Type;
    using rk_params_type = runge_kutta_parameters<value_type, s_stage_count>;

public:
    constexpr generic_runge_kutta_base(rk_params_type rk_params) noexcept
        : m_rk_params{ rk_params }
    {
    }

    constexpr generic_runge_kutta_base(
        size_type             n,
        rk_params_type const& rk_params
    ) noexcept
        : m_rk_params{ rk_params }
    {
        resize_internals(n);
    }

    [[nodiscard]]
    static constexpr auto stage_count() noexcept -> size_type
    {
        return Stage_Count;
    }

    auto do_step_impl(
        auto&&      system,
        state_type& x_in_out,
        time_type   t,
        time_type   dt
    ) noexcept -> void
    {
        if constexpr (data_types::dt_concepts::SizedInstance<deriv_type>)
        {
            assert(x_in_out.size() == m_x_tmp.size());
        }
        if constexpr (data_types::dt_concepts::SizedInstance<deriv_type> &&
                      data_types::dt_concepts::SizedInstance<state_type>)
        {
            for (auto const& dx : m_dxdt)
            {
                assert(x_in_out.size() == dx.size());
            }
        }

        system(x_in_out, m_dxdt[0], t);
        for (auto j = 1; j < s_stage_count; ++j)
        {
            const auto t_j = t + m_rk_params.c(j) * dt;
            m_x_tmp        = x_in_out;
            for (auto i = 0; i < j; ++i)
            {
                const auto a = m_rk_params.a(j, i);
                if (std::abs(a) < std::numeric_limits<value_type>::epsilon()) continue;
                m_x_tmp += m_dxdt[i] * (a * dt);
            }
            system(m_x_tmp, m_dxdt[j], t_j);
        }
        const auto& b = m_rk_params.b();
        x_in_out += dt * expr_reduce<Stage_Count>(m_dxdt, b);
        // x_in_out += dt * (m_dxdt[0] * b[0] + m_dxdt[1] * b[1] + m_dxdt[2] * b[2] +
        // m_dxdt[3] * b[3]);
    }

    auto resize_internals(size_type n) noexcept -> void
        requires data_types::dt_concepts::Resizeable<deriv_type> ||
                 data_types::dt_concepts::Resizeable<state_type>
    {
        assert(n > 0);
        if constexpr (data_types::dt_concepts::Resizeable<state_type>)
        {
            m_x_tmp.resize(n);
        }
        if constexpr (data_types::dt_concepts::Resizeable<deriv_type>)
        {
            for (auto& dx : m_dxdt)
            {
                dx.resize(n);
            }
        }
    }

private:
    rk_params_type m_rk_params;
    state_type     m_x_tmp;
    deriv_type     m_dxdt[Stage_Count];
};

/*
        struct print_butcher
        {
            const stage_vector_base &m_base;
            std::ostream &m_os;

            print_butcher( const stage_vector_base &base , std::ostream &os )
            : m_base( base ) , m_os( os )
            { }

            template<class Index>
            void operator()(Index) const {
                m_os << boost::fusion::at<Index>(m_base).c << " | ";
                for( size_t i=0 ; i<Index::value ; ++i )
                    m_os << boost::fusion::at<Index>(m_base).a[i] << " ";
                m_os << std::endl;
            }
        };
        template< typename T , size_t stage_number >
        void inline operator()( stage< T , stage_number > const &stage ) const
        //typename stage_fusion_wrapper< T , mpl::size_t< stage_number > ,
intermediate_stage >::type const &stage ) const
        {
            if( stage_number > 1 )
            {
#ifdef BOOST_MSVC
#pragma warning( disable : 4307 34 )
#endif
                system( x_tmp , F[stage_number-2].m_v , t + stage.c * dt );
#ifdef BOOST_MSVC
#pragma warning( default : 4307 34 )
#endif
            }
            //std::cout << stage_number-2 << ", t': " << t + stage.c * dt << std::endl;

            if( stage_number < StageCount )
                detail::template generic_rk_call_algebra< stage_number , Algebra >()(
algebra , x_tmp , x , dxdt , F , detail::generic_rk_scale_sum< stage_number , Operations ,
Value , Time >( stage.a , dt) );
            //                  algebra_type::template for_eachn<stage_number>( x_tmp , x
, dxdt , F ,
            //                          typename operations_type::template scale_sumn<
stage_number , time_type >( stage.a , dt ) ); else detail::template
generic_rk_call_algebra< stage_number , Algebra >()( algebra , x_out , x , dxdt , F ,
                        detail::generic_rk_scale_sum< stage_number , Operations , Value ,
Time >( stage.a , dt ) );
            //                algebra_type::template for_eachn<stage_number>( x_out , x ,
dxdt , F ,
            //                            typename operations_type::template scale_sumn<
stage_number , time_type >( stage.a , dt ) );
        }

    };

    generic_rk_algorithm( const coef_a_type &a , const coef_b_type &b , const coef_c_type
&c ) : m_stages( a , b , c ) { }

    template< class System , class StateIn , class DerivIn , class Time , class StateOut ,
class StateTemp , class Deriv > void inline do_step( Algebra &algebra , System system ,
const StateIn &in , const DerivIn &dxdt , Time t , StateOut &out , Time dt , StateTemp
&x_tmp , Deriv F[StageCount-1] ) const
    {
        typedef typename odeint::unwrap_reference< System >::type unwrapped_system_type;
        unwrapped_system_type &sys = system;
        boost::fusion::for_each( m_stages , calculate_stage<
                unwrapped_system_type , StateIn , StateTemp , DerivIn , Deriv , StateOut ,
Time > ( algebra , sys , in , dxdt , out , x_tmp , F , t , dt ) );
    }

private:
    stage_vector m_stages;
*/

} // namespace solvers::explicit_stepers
