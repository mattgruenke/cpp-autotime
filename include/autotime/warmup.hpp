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

        The interface of this class usses clock speeds normalized by the
        reported peak turbo speed of the CPU.
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

        //! Gets minimum normalized clock speed threshold.
    virtual double minClockSpeed() const = 0;

        //! Sets minimum normalized clock speed threshold.
    virtual void minClockSpeed(
        double thresh   //!< Minimum threshold as normalized value.
    ) = 0;

        //! Gets maximum allowable clock speed regression from peak sample.
    virtual double maxClockSpeedDecrease() const = 0;

        //! Sets maximum allowable clock speed regression from peak sample.
    virtual void maxClockSpeedDecrease(
        double thresh   //!< Maximum threshold as normalized value.
    ) = 0;
};


} // namespace autotime


#endif // ndef AUTOTIME_WARMUP_HPP

