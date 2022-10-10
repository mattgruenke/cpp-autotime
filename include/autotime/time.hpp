////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Declares low-level timing functions.
/*! @file

    These perform direct timing measurements of N invocations of a given
    function.  They comprise the basic foundation upon which autotime is based.
    As such, these are probably not of much interest to end users.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_TIME_HPP
#define AUTOTIME_TIME_HPP


#include <autotime/types.hpp>

#include <autotime/detail/time_impl.hpp>

#include <functional>


namespace autotime
{


    //! Convenience function for creating a timer object for f().
Timer MakeTimer(
    const std::function< void() > f     //!< Function to measure.
);


    //! Convenience function for creating a timer object for f().
Timer MakeTimer(
    void (*f)()                         //!< Function to measure.
);


    //! Measures f() over a given number of iterations.
    /*!
        @returns the duration of num_iter calls of f().
    */
Durations Time(
    const std::function< void() > &f,   //!< Function to measure.
    int num_iter                        //!< Number of iterations to measure.
);


    //! Accumulates timing data from repeated calls to an instrumented function.
    /*!
        @returns the accumulated durations from num_iter calls of f().
    */
Durations Time(
    const std::function< Durations() > &f,  //!< Instrumented function.
    int num_iter                        //!< Number of iterations to measure.
);


    //! Measures f() over a given number of iterations.
    /*!
        @returns the duration of num_iter calls of f().
    */
Durations Time(
    void (*f)(),                        //!< Function to measure.
    int num_iter                        //!< Number of iterations to measure.
);


    //! Measures inst->f() over a given number of iterations.
    /*!
        @returns the time taken by num_iter of f().
    */
template<
    typename T
>
Durations Time(
    void(T::*f)() const,                //!< Member function to measure.
    const T *inst,                      //!< Instance on which to invoke f().
    int num_iter                        //!< Number of iterations to measure.
)
{
    return detail::TimeMember(
        reinterpret_cast< void(detail::ProxyType::*)() const >( f ),
        reinterpret_cast< const detail::ProxyType * >( inst ),
        num_iter );
}


    //! Intermediate state of Time().
struct TimePoints
{
    steady_clock::time_point real;
    thread_clock::time_point thread;
};


    //! Low-level function: samples clocks (e.g. at the beginning of Time()).
    /*!
        This is exported primarily for benchmarks that don't map well to
        the Time() measurement model.
    */
TimePoints Start();


    //! Low-level function: computes durations (e.g. at the end of Time()).
Durations End(
    const TimePoints &start             //!< Values returned by Start().
);


} // namespace autotime


#endif // ndef AUTOTIME_TIME_HPP

