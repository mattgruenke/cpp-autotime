////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! AutoTime Benchmark Collection.
/*! @file

    This tool provides a set of fundamental benchmarks, using libautotime for measurement.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/autotime.hpp"
#include "autotime/iterate.hpp"
#include "autotime/log.hpp"
#include "autotime/os.hpp"
#include "autotime/time.hpp"
#include "autotime/warmup.hpp"
#include "autotime/work.hpp"

#include <cstdio>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <thread>

#include <sys/ioctl.h>

#include <boost/optional.hpp>
#include <boost/program_options.hpp>

#include "description.hpp"
#include "dispatch.hpp"
#include "list.hpp"
#include "output.hpp"
#include "thread_utils.hpp"


using namespace autotime;
using namespace bench;


static int GetTermWidth()
{
    winsize ws;
    return (ioctl( fileno( stdout ), TIOCGWINSZ, &ws ) == 0) ? ws.ws_col : 80;
}


static int AutoselectSecondaryCoreId( int core0 )
{
    int core1 = -1;
    int choose_core1_attempt = 0;
    while (core1 == -1)
    {
        if (choose_core1_attempt++ >= 3)
        {
            core1 = core0;
            std::cerr
                << "Warning:\n"
                << "  Core autoselection picked core " << core1
                << " for the secondary thread that the\n"
                << "  primary will also use.  Multithreaded benchmarks might be impaired.\n\n";
            break;
        }

        // Let the scheduler pick which core to use for the secondary,
        //  as long as it differs from the primary.
        std::promise< void > done_promise;
        std::future< void > done_future = done_promise.get_future();
        std::thread thread{ [core0, &core1, &done_promise]()
            {
                int c = GetCurrentCoreId();

                // Perhaps a better way to do this would be to set a full affinity mask,
                //  but exclude core0 and any of its SMT siblings.
                if (c != core0) core1 = c;

                done_promise.set_value();
            } };

        // Keep the current thread busy, while waiting for the child thread,
        //  to minimize the chance of it getting run on the same core.
        constexpr auto d = std::chrono::seconds::zero();
        while (std::future_status::ready != done_future.wait_for( d )) Mandelbrot( 0.1f, 256 );

        thread.join();
    }

    return core1;
}


int main( int argc, char *argv[] )
{
    // Defaults
    bool verbose = false;
    int core0 = -1;
    int core1 = -1;
    double warmup_min = 0.875;
    double warmup_slop = 0.125;
    int warmup_limit_ms = 125;
    std::string spec = "all";
    boost::optional< ListMode > list_mode;
    boost::optional< ListMode > describe_mode;
    bool run = false;
    Format format = Format::pretty;

    // Parse commandline options.
    namespace prog_opts = boost::program_options;
    prog_opts::options_description desc( "Allowed options", GetTermWidth() );
    std::string list_modes = List< ListMode >( ", " );
    std::string list_help =
        "Enumerate selected benchmarks or categories.  (options: " + list_modes + ").";
    std::string describe_help =
        "Print detailed info about benchmarks, categories.  (options: " + list_modes + ").";
    std::string format_help = "Output format (options: " + List< Format >( ", " ) + ").";
    desc.add_options()
        ( "help", "Show help message and exit." )
        ( "verbose",
          prog_opts::bool_switch( &verbose ),
          "Print informational messages to stderr." )
        ( "debug",
          prog_opts::bool_switch()->notifier(
            []( const bool &val ){ if (val) DebugLog( &std::cerr ); }
          ),
          "Print debugging messages to stderr." )
        ( "core",
          prog_opts::value( &core0 )->value_name( "N" ),
          "Which core to use (-1 -> auto)." )
        ( "coreB",
          prog_opts::value( &core1 )->value_name( "N" ),
          "Which core to use for secondary thread (-1 -> auto)." )
        ( "warmup-limit",
          prog_opts::value( &warmup_limit_ms )->value_name( "ms" )->default_value( warmup_limit_ms ),
          "Core warmup time limit." )
        ( "warmup-target",
          prog_opts::value( &warmup_min )->value_name( "F" )->default_value( warmup_min ),
          "Core warmup normalized frequency threshold." )
        ( "warmup-slop",
          prog_opts::value( &warmup_slop )->value_name( "F" )->default_value( warmup_slop ),
          "Core warmup normalized frequency regression limit." )
        ( "select",
          prog_opts::value( &spec )->value_name( "spec" )->default_value( spec ),
          "Specifies the set of benchmarks (see below)." )
        ( "list",
          prog_opts::value< ListMode >()->value_name( "mode" )->implicit_value( ListMode::joint )->
            notifier( [&list_mode]( const ListMode &mode ){ list_mode = mode; } ),
          list_help.c_str() )
        ( "describe",
          prog_opts::value< ListMode >()->value_name( "mode" )->implicit_value( ListMode::joint )->
            notifier( [&describe_mode]( const ListMode &mode ){ describe_mode = mode; } ),
          describe_help.c_str() )
        ( "run",
          prog_opts::bool_switch( &run ),
          "Perform the benchmarks." )
        ( "output-format",
          prog_opts::value( &format )->value_name( "fmt" )->default_value( format ),
          format_help.c_str() )
    ;

    namespace cli_style = prog_opts::command_line_style;
    const int style = cli_style::unix_style & ~cli_style::allow_guessing;
    prog_opts::parsed_options parsed = prog_opts::parse_command_line( argc, argv, desc, style );
    //std::vector< std::string > unrecognized =
    //    prog_opts::collect_unrecognized( parsed.options, prog_opts::include_positional );
    prog_opts::variables_map vm;
    prog_opts::store( parsed, vm );
    prog_opts::notify( vm );

    if (vm.count( "help" ))
    {
        std::cout
            << "\nAutoTime Benchmarking CLI\n"
            << "\nUsage: " << basename( argv[0] ) << " [options]\n"
            << "\n" << desc << "\n";
        return 0;
    }

    std::set< Benchmark > benchmarks = ParseSpecification( spec );

    if (list_mode)
    {
        PrintList( std::cout, benchmarks, *list_mode ) << "\n";

        // --run is implied only if --list is absent.
        if (!run && !describe_mode) return 0;
    }

    if (describe_mode)
    {
        PrintDescriptions( std::cout, benchmarks, *describe_mode ) << "\n";

        // Unless otherwise specified, this overrides --run.
        if (!run) return 0;
    }

    // Find out what core the main thread will be using, to ensure the secondary is different.
    if (core0 == -1) core0 = GetCurrentCoreId();

    if (core1 == -1) core1 = AutoselectSecondaryCoreId( core0 );

    // Try to stay on a specific core - must follow picking a core1 to avoid that thread
    //  inheriting the affinity we're setting for this one.
    SetCoreAffinity( core0 );
    if (verbose) std::cerr << "Running on core " << core0 << "\n";

    SetSecondaryCoreId( core1 );
    if (verbose) std::cerr << "Secondary on core " << core1 << "\n";

    // Try to warmup the core to near-peak clock speed.
    std::unique_ptr< ICoreWarmupMonitor > warmupMonitor = ICoreWarmupMonitor::create( core0 );
    warmupMonitor->minClockSpeed( warmup_min );
    warmupMonitor->maxClockSpeedDecrease( warmup_slop );

    steady_clock::time_point warmup_start = steady_clock::now();
    steady_clock::time_point warmup_finish =
        IterateUntil(
            [](){ Mandelbrot( 0.1f, 256 ); },
            warmup_start + std::chrono::milliseconds{ warmup_limit_ms },
            std::chrono::milliseconds{ 1 },
            std::bind( &ICoreWarmupMonitor::operator(), warmupMonitor.get() ) );

    steady_clock::duration warmup_dur = warmup_finish - warmup_start;
    auto warmup_dur_us = std::chrono::duration_cast< std::chrono::microseconds >( warmup_dur );
    if (verbose) std::cerr << "\nWarmup completed after " << warmup_dur_us.count() / 1000.0 << " ms.\n";

    // Setup output handler.
    std::unique_ptr< IOutputFormatter > output = IOutputFormatter::create( std::cout, format );

    // Run the specified benchmarks.
    for (Benchmark benchmark: benchmarks)
    {
        BenchTimers timers = MakeTimers( benchmark );

        // Time the function & its overhead.
        DurationsForIters exp_dfi = AutoTime( timers.primary );
        DurationsForIters ovh_dfi{};
        if (timers.overhead) ovh_dfi = AutoTime( timers.overhead );

        // Postprocess and display the results.
        CpuClockPeriod core_speed = GetCoreClockTick( core0 );
        NormDurations norm = exp_dfi.normalize() - ovh_dfi.normalize();
        output->write( benchmark, norm, exp_dfi.num_iters, core_speed /*, warnings */ );
    }

    return 0;
}

