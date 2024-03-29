////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements low-level timing functions.
/*! @file

    See time.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/time.hpp"
#include "autotime/autotime.hpp"
#include "internal.hpp"

#include <mutex>


namespace autotime
{


template<
    typename clock_type
>
static Durations GetOverhead()
{
    static std::recursive_mutex mutex;
    std::lock_guard< std::recursive_mutex > lock{ mutex };

    static Durations overhead{};
    static bool called = false;
    if (!called)
    {
        called = true;

        autotime::Timer timer =
            []( int num_iters )
            {
                return autotime::Time( &clock_type::now, num_iters );
            };

        auto dfi = autotime::AutoTime( timer );

        NormDurations norm = dfi.normalize();
        overhead =
            {
                std::chrono::duration_cast< steady_clock::duration >( norm.real ),
                std::chrono::duration_cast< steady_clock::duration >( norm.thread )
            };

        AUTOTIME_DEBUG( AUTOTIME_TYPENAME( clock_type ) << ": " << overhead.real.count() << " ns" );
    }

    return overhead;
}


TimePoints Start()
{
    // Sample real time last, in order to maximize its accuracy.
    thread_clock::time_point thread = thread_clock::now();
    steady_clock::time_point real = steady_clock::now();

    return { real, thread };
}


Durations End( const TimePoints &start )
{
    // Sample real time first, in order to maximize its accuracy.
    steady_clock::time_point real_time = steady_clock::now();
    thread_clock::time_point thread_time = thread_clock::now();

    Durations steady_overhead = GetOverhead< steady_clock >();
    steady_clock::duration real_dur = real_time - start.real - steady_overhead.real;

    // Note: thread duration spans two calls to steady_clock::now().
    Durations thread_overhead = GetOverhead< thread_clock >();
    thread_clock::duration thread_dur = thread_time - start.thread
        - 2 * steady_overhead.thread - thread_overhead.thread;

    return { real_dur, thread_dur };
}


Durations Time( const std::function< void() > &f, int num_iter )
{
    TimePoints start_times = Start();

    for (int i = 0; i < num_iter; ++i) f();

    return End( start_times );
}


Durations Time( const std::function< Durations() > &f, int num_iter )
{
    Durations durs{};

    for (int i = 0; i < num_iter; ++i) durs += f();

    return durs;
}


Durations Time( void (*f)(), int num_iter )
{
    TimePoints start_times = Start();

    for (int i = 0; i < num_iter; ++i) f();

    return End( start_times );
}


Durations detail::TimeMember(
    void(detail::ProxyType::*f)() const, const detail::ProxyType *inst, int num_iter )
{
    TimePoints start_times = Start();

    for (int i = 0; i < num_iter; ++i) (inst->*f)();

    return End( start_times );
}


Timer MakeTimer( const std::function< void() > f )
{
    return [f]( int num_iters )
        {
            return Time( f, num_iters );
        };
}


Timer MakeTimer( void (*f)() )
{   
    return [f]( int num_iters )
        {   
            return Time( f, num_iters );
        };
}


} // namespace autotime

