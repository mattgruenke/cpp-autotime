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
#include <string>


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
        //! Indicates the state of the core warmup procedure.
    enum class status
    {
        incomplete, //!< Clockspeed not checked or below minimum ratio.
        complete,   //!< Clockspeed met or exceeded minimum ratio.
        aborted     //!< Clockspeed was reduced between consecutive polls.
    };

        //! Constructs a new instance.
    static std::unique_ptr< ICoreWarmupMonitor > create(
        int coreId=-1   //!< Core on which this is to be used (-1 -> current).
    );

    virtual ~ICoreWarmupMonitor();

#if 0
        //! Whether core is still in warmup phase.
        /*!
            @returns false if threshold is met or clock speed decreases.
        */
    bool operator()() = 0;
#endif

        //! Checks the warmup status of the current core.
        /*!
            @throws std::runtime_error if the process couldn't be completed.
        */
    virtual status check() = 0;

        //! Returns the result of the previous call to check().
    virtual status peek() const = 0;

        //! Returns a detailed message, describing the current status.
    virtual std::string details() const = 0;

        //! Gets minimum clock speed ratio.
    virtual float minClockSpeedRatio() const = 0;

        //! Sets minimum clock speed ratio.
    virtual void minClockSpeedRatio(
        float ratio     //!< Minimum ratio of clock speed to peak turbo.
    ) = 0;
};


} // namespace autotime


#endif // ndef AUTOTIME_WARMUP_HPP

