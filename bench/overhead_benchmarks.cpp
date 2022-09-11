////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements factory functions for measuring test framework overheads.
/*! @file

    These benchmarks provide information about the degree of measurement
    overhead that's typically being subtracted off, to give an appreciation for
    the amount of error that might be added, in the process.
    See autotime/overhead.hpp, for further details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "dispatch.hpp"

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"


using namespace autotime;


namespace bench
{


template<> autotime::BenchTimers MakeTimers< Benchmark::overhead_nullary >()
{
    return { MakeTimer( &Overhead_void<> ), nullptr };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::overhead_nullary_fn >()
{
    return { MakeTimer( MakeOverheadFn< void >() ), nullptr };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::overhead_nullary_ptr >()
{
    return { MakeTimer( MakeOverheadPtr< void >() ), nullptr };
}


} // namespace bench

