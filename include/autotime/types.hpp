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


#include <autotime/clocks.hpp>

#include <chrono>


namespace autotime
{


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
};


    //! Bundles both the number of iterations and aggregate measurement.
struct DurationsForIters
{
    int num_iters = 0;
    Durations durs;

        //! Normalizes the durations by the number of iterations.
    NormDurations normalize() const;
};


} // namespace autotime


#endif // ndef AUTOTIME_TYPES_HPP

