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

#include <stdexcept>


namespace autotime
{


class CoreWarmupMonitor: public ICoreWarmupMonitor
{
public:
    CoreWarmupMonitor( int coreId );

    bool operator()() override;
    float minClockSpeedRatio() const override;
    void minClockSpeedRatio( float ratio ) override;

private:
    void checkCoreId() const;
    float getClockSpeedRatio() const;

    // Parameters:
    float minClockSpeedRatio_ = 0.0f;

    // Runtime state:
    const int coreId_ = -1;
    const cpu_clock_ticks minClockTick_;
    float ratio_ = 0.0f;
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


float CoreWarmupMonitor::getClockSpeedRatio() const
{
    const float min_tick_float = static_cast< float >( minClockTick_.count() );
    const float current = min_tick_float / GetCoreClockTick( coreId_ ).count();
    if (current < ratio_)
    {
        std::string message =
            "During warmup, core clock speed ratio dropped from " + std::to_string( ratio_ )
            + " to " + std::to_string( current );
        throw std::runtime_error( message );
    }
    return current;
}


bool CoreWarmupMonitor::operator()()
{
    this->checkCoreId();
    ratio_ = this->getClockSpeedRatio();
    return (ratio_ < minClockSpeedRatio_);
}


float CoreWarmupMonitor::minClockSpeedRatio() const
{
    return minClockSpeedRatio_;
}


void CoreWarmupMonitor::minClockSpeedRatio( float ratio )
{
    minClockSpeedRatio_ = ratio;
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

