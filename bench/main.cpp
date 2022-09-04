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

#include <functional>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "output.hpp"


using namespace autotime;
using namespace bench;


int main( int argc, char *argv[] )
{
    // Defaults
    bool verbose = false;
    int coreId = -1;
    double warmup_min = 0.875;
    double warmup_slop = 0.125;
    int warmup_limit_ms = 125;
    std::string spec = "all";
    Format format = Format::pretty;

    // Parse commandline options.
    namespace prog_opts = boost::program_options;
    prog_opts::options_description desc( "Allowed options" );
    std::string format_help = "Output format (supported values: " + List< Format >( ", " ) + ").";
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
          prog_opts::value( &coreId )->value_name( "N" ),
          "Which core to use (-1 -> auto)." )
        ( "warmup-limit",
          prog_opts::value( &warmup_limit_ms )->value_name( "ms" )->default_value( warmup_limit_ms ),
          "Core warmup time limit." )
        ( "warmup-target",
          prog_opts::value( &warmup_min )->value_name( "F" )->default_value( warmup_min ),
          "Core warmup normalized frequency threshold." )
        ( "warmup-slop",
          prog_opts::value( &warmup_slop )->value_name( "F" )->default_value( warmup_slop ),
          "Core warmup normalized frequency regression limit." )
        ( "run",
          prog_opts::value( &spec )->value_name( "spec" )->default_value( spec ),
          "Run the specified benchmarks." )
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

    // Try to stay on a specific core.
    coreId = SetCoreAffinity( coreId );
    if (verbose) std::cerr << "Running on core " << coreId << "\n";

    // Try to warmup the core to near-peak clock speed.
    std::unique_ptr< ICoreWarmupMonitor > warmupMonitor = ICoreWarmupMonitor::create( coreId );
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

    return 0;
}

