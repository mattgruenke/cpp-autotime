////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Defines common, public types used by this library.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_TYPES_HPP
#define AUTOTIME_TYPES_HPP


#include <chrono>
#include <functional>

#include <autotime/clocks.hpp>


namespace autotime
{


    //! CPU clock tick representation with enough range for 0.5 MHz to 1 THz.
using CpuClockPeriod = std::chrono::duration< int32_t, std::femto >;


    //! A bundle of timing information returned by Time().
struct Durations
{
    steady_clock::duration real;    //!< Cumulative realtime execution time.
    thread_clock::duration thread;  //!< Cumulative thread execution time.
};


    //! A bundle of timing information normalized by num_iters.
struct NormDurations
{
    // Higher resolution than nanoseconds is needed for cycle-accuracy.
    using duration = std::chrono::duration< int64_t, std::pico >;

    duration real;
    duration thread;

    NormDurations operator-( const NormDurations &rhs ) const;
};


    //! Bundles both the number of iterations and aggregate measurement.
struct DurationsForIters
{
    int num_iters;
    Durations durs;

        //! Normalizes the durations by the number of iterations.
    NormDurations normalize() const;
};


    //! An abstraction over Time().
    /*!
        This mechanism enables the timing subject + any requisite context to be
        passed into functions like AutoTime() and Estimate().

        Note that it must be callable multiple times, without side effects.
        So, each invocation must perform any needed initialization and cleanup.
    */
using Timer = std::function< Durations( int num_iters ) >;


    //! Pairs a primary timer with another timer to measure its overhead.
    /*!
        Since each benchmark can potentially have a different amount of
        overhead, it's defined not only by a primary timer, but also the
        timer for characterizing its overhead.
    */
struct BenchTimers
{
    Timer primary;
    Timer overhead;     //!< In rare cases, this may be null.
};


} // namespace autotime


#endif // ndef AUTOTIME_TYPES_HPP

