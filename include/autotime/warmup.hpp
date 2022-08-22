////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Declares interface for monitoring initial elevation of CPU clock speed.
/*! @file

    See class ICoreWarmupMonitor, for details.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_WARMUP_HPP
#define AUTOTIME_WARMUP_HPP


#include <memory>


namespace autotime
{


    //! Interface for monitoring the warmup process of a single core.
    /*!
        This is intended primarily to be used as the predicate function of
        IterateUntil().
    */
class ICoreWarmupMonitor
{
public:
        //! Constructs a new instance.
    static std::unique_ptr< ICoreWarmupMonitor > create(
        int coreId=-1   //!< Core on which this is to be used (-1 -> current).
    );

    virtual ~ICoreWarmupMonitor();

        //! Indicates whether core is still in warmup phase.
        /*!
            @returns false if threshold is met.

            @throws std::runtime_error if warmup process can't be completed.
        */
    virtual bool operator()() = 0;

        //! Gets minimum clock speed ratio.
    virtual float minClockSpeedRatio() const = 0;

        //! Sets minimum clock speed ratio.
    virtual void minClockSpeedRatio(
        float ratio     //!< Minimum ratio of clock speed to peak turbo.
    ) = 0;
};


} // namespace autotime


#endif // ndef AUTOTIME_WARMUP_HPP

