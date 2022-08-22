////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Declares functions used to iterate over a callback.
/*! @file

    These are initially used only for CPU core warmup.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_ITERATE_HPP
#define AUTOTIME_ITERATE_HPP


#include <functional>

#include <autotime/clocks.hpp>


namespace autotime
{


    //! Repeatedly calls a function until time t or a predicate returns false.
    /*!
        This is primarily intended for CPU warmup.  As such, we want to perform
        work in batches, rather than sampling the clock & predicate between each
        call.  The step parameter is used to specify the approximate batch size.

        @returns the time at which the function returned.
    */
steady_clock::time_point IterateUntil(
    const std::function< void() > &work,    //!< Function to execute.
    const steady_clock::time_point &t_limit,//!< Time limit.
    const steady_clock::duration &step,     //!< Approximate batch size of work.
    const std::function< bool() > &predicate//!< Controls whether to continue.
);


} // namespace autotime


#endif // ndef AUTOTIME_ITERATE_HPP

