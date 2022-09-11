////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements factory functions for work-category benchmarks.
/*! @file

    These benchmarks merely exist to provide information about the different
    warmup methods supported by libautotime.  See autotime/work.hpp, for
    further details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "dispatch.hpp"

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"
#include "autotime/work.hpp"


using namespace autotime;

namespace bench
{


template<> autotime::BenchTimers MakeTimers< Benchmark::fibonacci_u64 >()
{
        // No overhead to subtract, since the function is what we're measuring.
    return { MakeTimer( &LargestFibonacci< uint64_t > ), nullptr };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::mandelbrot_256 >()
{
    std::function< void() > f = [](){ Mandelbrot( 0.1f, 256 ); };
    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::trigonometry >()
{
    std::function< void() > f = [](){ TrigonometryFunctions( 0.1f ); };
    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


} // namespace bench

