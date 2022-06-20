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


template< typename T > static void Maximize( std::atomic< T > &cur, T newer )
{
    T c = cur;
    while (newer > c && !cur.compare_exchange_weak( c, newer )) c = cur;
}


static bool Matches( std::istream &in, const std::string &expected )
{
    auto next = expected.begin();
    while (in && next != expected.end() && in.get() == *next) ++next;
    return next == expected.end();
}


cpu_clock_ticks GetCoreClockTick( int core_id )
{
    if (core_id < 0) core_id = GetCurrentCoreId();

    std::vector< char > buffer; // buffer sholud have a larger scope than file.
    std::ifstream file;

    // If cpuinfo was previously read, configuer the buffer to support a one-shot read.
    static std::atomic< std::streamsize > max_size{ 0 };
    if (max_size)
    {
        AUTOTIME_DEBUG( "Configuring buffer based on previous max: " << max_size );
        buffer.resize( max_size * 5 / 4 );  // Add an extra 25%, for variable length fields.
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
            AUTOTIME_DEBUG( "Raw value: " << mhz );
            break;
        }
    }

    if (file && !max_size)  // Don't recompute, since file's size seems the same, every time.
    {
        // Since file isn't seekable, read to the end, to get the size.
        std::streamsize pos = file.tellg();
        file.ignore( unlimited ); 

        // Remember the largest size we've seen, to optimize future reads.
        Maximize( max_size, pos + file.gcount() );
    }

    if (!mhz)
    {
        AUTOTIME_ERROR( "failed to extract core MHz." );
        return {};
    }

    // To convert from MHz, scale by number of clock ticks per microsecond.
    double scale = std::chrono::microseconds{ 1 } / cpu_clock_ticks{ 1 };
    return cpu_clock_ticks{ lrint( scale / mhz ) };
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

    cpu_set_t *cpu_set = CPU_ALLOC( num_cpus );
    CPU_SET( core_id, cpu_set );

    const pid_t thread_id = 0;  // 0 -> current thread
    if (sched_setaffinity( thread_id, CPU_ALLOC_SIZE( num_cpus ), cpu_set ))
    {
        AUTOTIME_ERRNO( "failed to set affinity" );
    }

    CPU_FREE( cpu_set );

    return core_id;
}


} // namespace autotime

