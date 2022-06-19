////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Defines clock types used by autotime.
/*! @file

    The library uses two custom clock types: steady_clock and thread_clock.
    If there's value in doing so, a build-time option could be provided to
    use std::steady_clock instead, but the value offered by this custom
    implementation is that it uses CLOCK_MONOTONIC_RAW.  The current version of
    GCC's libstdc++ seems to use regular CLOCK_MONOTONIC.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_CLOCKS_HPP
#define AUTOTIME_CLOCKS_HPP


#include <autotime/detail/clock_impl.hpp>

#include <chrono>


namespace autotime
{


    //! Primary clock type used by this library.
    /*!
        std::chrono-compatible clock, implemented using CLOCK_MONOTONIC_RAW.
        Similar to std::steady_clock, but gcc's libstdc++ appears to use
        CLOCK_MONOTONIC.  That makes sense for long-running applications, but
        slightly disadvantages short-running uses like ours.
    */
struct steady_clock
{
    using duration = std::chrono::nanoseconds;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point< steady_clock, duration >;

    static constexpr bool is_steady = true;

    static time_point now() noexcept;
};


    //! Represents cumulative amount of a thread's CPU utilization.
    /*!
        std::chrono-compatible clock, implemented using CLOCK_THREAD_CPUTIME_ID.
        As such, it should exclude time the thread spends blocked on I/O or
        waiting on other threads.
    */
struct thread_clock
{
    using duration = std::chrono::nanoseconds;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point< thread_clock, duration >;

    static constexpr bool is_steady = true; // Not sure about this.

    static time_point now() noexcept;
};


// struct steady_clock:
inline steady_clock::time_point steady_clock::now() noexcept
{
    return detail::now< steady_clock, CLOCK_MONOTONIC_RAW >();
}


// struct thread_clock:
inline thread_clock::time_point thread_clock::now() noexcept
{
    return detail::now< thread_clock, CLOCK_THREAD_CPUTIME_ID >();
}


} // namespace autotime


#endif // ndef AUTOTIME_CLOCKS_HPP

