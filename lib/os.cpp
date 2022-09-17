////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements OS-dependent functionality.
/*! @file

    See os.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/os.hpp"
#include "internal.hpp"

#include <atomic>
#include <cmath>
#include <fstream>
#include <limits>
#include <vector>

#include <sched.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>


namespace autotime
{


std::chrono::nanoseconds GetTimeslice()
{
    std::ifstream file{ "/proc/sys/kernel/sched_min_granularity_ns" };

    if (file)
    {
        int ns = 0;
        file >> ns;
        if (file) return std::chrono::nanoseconds{ ns };
    }

    return {};
}


static bool Matches( std::istream &in, const std::string &expected )
{
    auto next = expected.begin();
    while (in && next != expected.end() && in.get() == *next) ++next;
    return next == expected.end();
}


CpuClockPeriod GetCoreClockTick( int core_id )
{
    if (core_id < 0) core_id = GetCurrentCoreId();

    std::vector< char > buffer; // buffer sholud have a larger scope than file.
    std::ifstream file;

    // If cpuinfo was previously read, configure the buffer to support a one-shot read.
    static std::atomic< std::streamsize > prev_size{ 0 };
    if (prev_size)
    {
        buffer.resize( prev_size * 5 / 4 );  // Add an extra 25%, for variable length fields.
        file.rdbuf()->pubsetbuf( buffer.data(), buffer.size() );
    }

    // Now that any buffer has been configured, the file can finally be opened.
    file.open( "/proc/cpuinfo" );
    if (!file)
    {
        AUTOTIME_ERROR( "failed to open /proc/cpuinfo" );
        return {};
    }

    double mhz = 0.0;
    int count = 0;
    constexpr auto unlimited = std::numeric_limits< std::streamsize >::max();
    while (file)
    {
        file.ignore( unlimited, '\n' );
        if (Matches( file, "cpu MHz" ) && count++ == core_id)
        {
            file.ignore( unlimited, ':' );
            file >> mhz;
            AUTOTIME_DEBUG( "(double) cpu MHz: " << mhz );
            break;
        }
    }

    if (file && !prev_size)  // Don't recompute, since file's size seems the same, every time.
    {
        // Since file isn't seekable, read to the end, to get the size.
        std::streamsize pos = file.tellg();
        file.ignore( unlimited ); 

        // Remember the previous size, to optimize future reads.
        prev_size = pos + file.gcount();
        AUTOTIME_DEBUG( "Size of /proc/cpuinfo is " << prev_size );
    }

    if (!mhz)
    {
        AUTOTIME_ERROR( "failed to extract core MHz." );
        return {};
    }

    // To convert from MHz, scale by number of clock ticks per microsecond.
    double scale = std::chrono::microseconds{ 1 } / CpuClockPeriod{ 1 };
    return CpuClockPeriod{ lrint( scale / mhz ) };
}


CpuClockPeriod GetCoreMinClockTick( int core_id )
{
    if (core_id < 0) core_id = GetCurrentCoreId();

    std::string cpu = std::to_string( core_id );
    std::string filename = "/sys/devices/system/cpu/cpufreq/policy" + cpu + "/scaling_max_freq";
    std::ifstream file{ filename };

    if (file)
    {
        int khz = 0;
        file >> khz;
        if (file)
        {
            // To convert from kHz, scale by number of clock ticks per millisecond.
            int64_t scale = std::chrono::milliseconds{ 1 } / CpuClockPeriod{ 1 };
            AUTOTIME_DEBUG( "Scaling min clock tick by " << scale );
            return CpuClockPeriod{ scale / khz };
        }
    }

    return {};
}


int GetCurrentCoreId()
{
    int core_id = sched_getcpu();
    if (core_id < 0) AUTOTIME_ERRNO( "failed to get cpu" );
    return core_id;
}


int SetCoreAffinity( int core_id )
{
    const int num_cpus = get_nprocs_conf();
    if (num_cpus <= 0)
    {
        AUTOTIME_ERRNO( "failed to query ncpus" );
        return -1;
    }

    bool need_switch = false;
    if (core_id >= num_cpus)
    {
        AUTOTIME_ERROR( "specified out-of-range CPU ID." );
        return -1;
    }
    else if (core_id < 0)
    {
        core_id = GetCurrentCoreId();
        if (core_id < 0) return -1;
    }
    else need_switch = (GetCurrentCoreId() != core_id);

    cpu_set_t *cpu_set = CPU_ALLOC( num_cpus );
    CPU_SET( core_id, cpu_set );

    const pid_t thread_id = 0;  // 0 -> current thread
    if (sched_setaffinity( thread_id, CPU_ALLOC_SIZE( num_cpus ), cpu_set ))
    {
        AUTOTIME_ERRNO( "failed to set affinity" );
    }

    CPU_FREE( cpu_set );

    if (need_switch)
    {
        if (sched_yield()) AUTOTIME_ERRNO( "sched_yield() failed" );

        if (GetCurrentCoreId() != core_id) AUTOTIME_ERROR( "Not running on specified core" );
    }

    return core_id;
}


} // namespace autotime

