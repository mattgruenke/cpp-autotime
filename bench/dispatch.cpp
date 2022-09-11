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

    CASE__MAKE_TIMERS( memcpy_256 );
    CASE__MAKE_TIMERS( memcpy_4k );
    CASE__MAKE_TIMERS( memcpy_64k );
    CASE__MAKE_TIMERS( memcpy_1M );
    CASE__MAKE_TIMERS( memcpy_16M );
    CASE__MAKE_TIMERS( memcpy_256M );
    CASE__MAKE_TIMERS( strlen_256 );
    CASE__MAKE_TIMERS( strlen_4k );
    CASE__MAKE_TIMERS( strlen_64k );
    CASE__MAKE_TIMERS( strlen_1M );
    CASE__MAKE_TIMERS( strlen_16M );
    CASE__MAKE_TIMERS( strlen_256M );
    CASE__MAKE_TIMERS( memset_256 );
    CASE__MAKE_TIMERS( memset_4k );
    CASE__MAKE_TIMERS( memset_64k );
    CASE__MAKE_TIMERS( memset_1M );
    CASE__MAKE_TIMERS( memset_16M );
    CASE__MAKE_TIMERS( memset_256M );

    CASE__MAKE_TIMERS( overhead_nullary );
    CASE__MAKE_TIMERS( overhead_nullary_fn );
    CASE__MAKE_TIMERS( overhead_nullary_ptr );

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

