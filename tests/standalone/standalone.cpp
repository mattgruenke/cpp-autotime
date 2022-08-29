////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at //   http://www.boost.org/LICENSE_1_0.txt)
//
//! Simple test program for validating the libautotime installation and CMake Find Module.
/*! @file

    This isn't intended to do anything very useful.  See tools/, for better and more interesting
    examples of how to use the autotime library.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <autotime/autotime.hpp>
#include <autotime/iterate.hpp>
#include <autotime/os.hpp>
#include <autotime/time.hpp>
#include <autotime/warmup.hpp>
#include <autotime/work.hpp>

#include <iostream>


int main()
{
    // Try to stay on a specific core.
    const int coreId = autotime::SetCoreAffinity();
    std::cout << "Running on core " << coreId << "\n";

    // Try to warmup the core to near-peak clock speed.
    std::unique_ptr< autotime::ICoreWarmupMonitor > warmupMonitor
        = autotime::ICoreWarmupMonitor::create( coreId );
    warmupMonitor->minClockSpeed( 0.85 );

    autotime::steady_clock::time_point warmup_start = autotime::steady_clock::now();
    autotime::steady_clock::time_point warmup_finish =
        autotime::IterateUntil(
            [](){ autotime::Mandelbrot( 0.1f, 256 ); },
            warmup_start + std::chrono::milliseconds{ 125 },
            std::chrono::milliseconds{ 1 },
            std::bind( &autotime::ICoreWarmupMonitor::operator(), warmupMonitor.get() ) );

    autotime::steady_clock::duration warmup_dur = warmup_finish - warmup_start;
    auto warmup_dur_us = std::chrono::duration_cast< std::chrono::microseconds >( warmup_dur );
    std::cout << "\nWarmup completed after " << warmup_dur_us.count() / 1000.0 << " ms.\n";

    // Time a simple function.
    autotime::Timer mandle_timer =
        []( int num_iters )
        {
            void (*f)() = [](){ autotime::Mandelbrot( 0.1f, 256 ); };
            return autotime::Time( f, num_iters );
        };
    autotime::DurationsForIters dfi = autotime::AutoTime( mandle_timer );

    // Postprocess and display the results.
    autotime::NormDurations norm = dfi.normalize();
    std::cout << "\nmandle() -> "
        << "{ real: " << dfi.durs.real.count() << " ns, "
        << "thread: " << dfi.durs.thread.count() << " ns } "
        << "for " << dfi.num_iters << " iters = " << norm.real.count() / 1000000.0 << " us.\n";

    return 0;
}

