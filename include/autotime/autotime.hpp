////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Declares top-level AutoTime() API functions.
/*! @file

    The AutoTime() functions estimate the optimal number of iterations to use,
    then measure the specified subject over that amount.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_AUTOTIME_HPP
#define AUTOTIME_AUTOTIME_HPP


#include <autotime/types.hpp>


namespace autotime
{


    //! Automatically determines the optimal number of iterations over which to
    //!  execute a given subject and returns that result.
    /*!
        @returns the number of iterations ultimately used and how long it took.
    */
DurationsForIters AutoTime(
    const Timer &t      //!< Wrapped function to measure.
);


} // namespace autotime


#endif // ndef AUTOTIME_AUTOTIME_HPP

