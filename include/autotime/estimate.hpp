////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Declares functions for estimating iteration counts; needed by AutoTime().
/*! @file

    This is a lower-level facility, probably not of much interest to end users.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_ESTIMATE_HPP
#define AUTOTIME_ESTIMATE_HPP


#include <autotime/types.hpp>


namespace autotime
{


    //! Estimates number of iterations of f() needed to span a target duration.
    /*!
        @returns the estimated number of iterations and their duration.

        This function attempts to find the number of iterations needed for the
        subject to take at least 0.8 * target and no more than 2.0 * target.

        @note
        For long-running functions, this will exit early.  The current
        threshold is any function taking longer than 1/3rd of the target,
        which is tied to the tolerances used to determine convergence.
    */
DurationsForIters Estimate(
    const Timer &timer,                     //!< Wrapped function to measure.
    const steady_clock::duration &target    //!< Target duration to estimate.
);


} // namespace autotime


#endif // ndef AUTOTIME_ESTIMATE_HPP

