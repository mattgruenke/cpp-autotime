////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements the runtime dispatch function for creating per-benchmark timers.
/*! @file

    See dispatch.hpp, for more information.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "dispatch.hpp"

#include <stdexcept>


namespace bench
{


autotime::BenchTimers MakeTimers( Benchmark benchmark )
{
    switch (benchmark)
    {
#define CASE__MAKE_TIMERS( n ) \
    case Benchmark::n: \
        return MakeTimers< Benchmark::n >()

    CASE__MAKE_TIMERS( fibonacci_u64 );
    CASE__MAKE_TIMERS( mandelbrot_256 );
    CASE__MAKE_TIMERS( trigonometry );

    default:
        break;

#undef CASE__MAKE_TIMERS
    }

    throw std::runtime_error( "Unimplemented: " + ToStr( benchmark ) );
}


} // namespace bench

