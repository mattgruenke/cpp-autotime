////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Defines interface for OS-dependent functionality.
/*! @file

    The initial focus has been to support mainstream Linux configurations
    (i.e. CFS scheduler).  Contributions of spport for other schedulers and
    operating systems is welcome.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_OS_HPP
#define AUTOTIME_OS_HPP


#include <chrono>

#include <autotime/types.hpp>


namespace autotime
{


    //! Returns the approximate minimum interval between preemptions.
    /*!
        @returns 0, if no supported scheduler type is found.
    */
std::chrono::nanoseconds GetTimeslice();


    //! Queries the duration of a CPU core clock tick, in femtoseconds.
    /*!
        Specify -1 to query the core on which the current thread is running.

        @returns 0, if the clock speed couldn't be determined.
    */
CpuClockPeriod GetCoreClockTick(
    int core_id=-1  //!< Specifies which core to query.
);


    //! Queries the minimum duration of a CPU core clock tick, in femtoseconds.
    /*!
        Specify -1 to query the core on which the current thread is running.

        @returns 0, if the max clock speed couldn't be determined.
    */
CpuClockPeriod GetCoreMinClockTick(
    int core_id=-1  //!< Specifies which core to query.
);


    //! Returns the ID number of the current CPU core.
int GetCurrentCoreId();


    //! Sets the affinity of the current thread to a specified core.
    /*!
        @returns -1, in case of failure.  Otherwise, the ID of the target core.
    */
int SetCoreAffinity(
    int core_id=-1  //!< Specifies which core to use (defaults to current).
);


} // namespace autotime


#endif // ndef AUTOTIME_OS_HPP

