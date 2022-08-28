////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements interface for monitoring initial elevation of CPU clock speed.
/*! @file

    See warmup.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/warmup.hpp"
#include "autotime/os.hpp"

#include <algorithm>
#include <stdexcept>


namespace autotime
{


using cpu_clock_ticks_dbl = std::chrono::duration< double, std::nano >;


class CoreWarmupMonitor: public ICoreWarmupMonitor
{
public:
    CoreWarmupMonitor( int coreId );

    bool operator()() override;

    double minClockSpeed() const override;
    void minClockSpeed( double thresh ) override;

    double maxClockSpeedDecrease() const override;
    void maxClockSpeedDecrease( double thresh ) override;

private:
    void checkCoreId() const;
    double getClockSpeedRatio() const;

    // Parameters:
    double minClockSpeed_ = 0.0;
    double maxDecrease_ = 0.01;

    // Runtime state:
    const int coreId_ = -1;
    const cpu_clock_ticks_dbl minClockTick_;
    double peak_ = 0.0;
};


CoreWarmupMonitor::CoreWarmupMonitor( int coreId )
:
    coreId_( (coreId < 0) ? GetCurrentCoreId() : coreId ),
    minClockTick_( GetCoreMinClockTick( coreId_ ) )
{
}
    

void CoreWarmupMonitor::checkCoreId() const
{
    const int current = GetCurrentCoreId();
    if (coreId_ != current)
    {
        std::string message =
            "During warmup, core ID changed from " + std::to_string( coreId_ )
            + " to " + std::to_string( current );
        throw std::runtime_error( message );
    }
}


double CoreWarmupMonitor::getClockSpeedRatio() const
{
    const double current = minClockTick_ / GetCoreClockTick( coreId_ );
    if (current < peak_ - maxDecrease_)
    {
        std::string message =
            "During warmup, core clock speed ratio dropped from " + std::to_string( peak_ )
            + " to " + std::to_string( current );
        throw std::runtime_error( message );
    }
    return current;
}


bool CoreWarmupMonitor::operator()()
{
    this->checkCoreId();
    peak_ = std::max( peak_, this->getClockSpeedRatio() );
    return (peak_ < minClockSpeed_);
}


double CoreWarmupMonitor::minClockSpeed() const
{
    return minClockSpeed_;
}


void CoreWarmupMonitor::minClockSpeed( double thresh )
{
    minClockSpeed_ = thresh;
}


double CoreWarmupMonitor::maxClockSpeedDecrease() const
{
    return maxDecrease_;
}


void CoreWarmupMonitor::maxClockSpeedDecrease( double thresh )
{
    maxDecrease_ = thresh;
}



// class ICoreWarmupMonitor:
std::unique_ptr< ICoreWarmupMonitor > ICoreWarmupMonitor::create( int coreId )
{
    return std::unique_ptr< ICoreWarmupMonitor >( new CoreWarmupMonitor{ coreId } );
}


ICoreWarmupMonitor::~ICoreWarmupMonitor()
{
}


} // namespace autotime

