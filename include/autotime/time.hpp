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


#include <autotime/clocks.hpp>

#include <autotime/detail/time_impl.hpp>

#include <functional>


namespace autotime
{


    //! A bundle of timing information returned by Time().
struct Durations
{
    steady_clock::duration real;    //!< Cumulative realtime execution time.
    thread_clock::duration thread;  //!< Cumulative thread execution time.
};


    //! Measures f() over a given number of iterations.
    /*!
        @returns the duration of num_iter calls of f().
    */
Durations Time(
    const std::function< void() > &f,   //!< Function to measure.
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
    return TimeMember(
        reinterpret_cast< void(detail::ProxyType::*)() const >( f ),
        reinterpret_cast< const detail::ProxyType * >( inst ),
        num_iter );
}


} // namespace autotime


#endif // ndef AUTOTIME_TIME_HPP

