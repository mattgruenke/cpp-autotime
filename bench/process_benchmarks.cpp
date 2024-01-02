////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements benchmarks related to subprocesses.
/*! @file

    See Category::process, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "description.hpp"
#include "dispatch.hpp"
#include "error_utils.hpp"
#include "list.hpp"
#include "format_utils.hpp"


using namespace autotime;

namespace bench
{


template<> Description Describe< Category::process >()
{
    Description desc;
    desc.measures = "Subprocess operations.";
    return desc;
}


static void WaitPid( pid_t pid )
{
    int wstatus = 0;
    constexpr int options = 0;
    while (waitpid( pid, &wstatus, options ) < 0)
    {
        if (errno != EINTR) throw_system_error( errno, "waitpid()" );
    }

    if (!WIFEXITED( wstatus )) throw std::runtime_error( "subprocess exited abnormally" );
    if (WEXITSTATUS( wstatus )) throw std::runtime_error( "nonzero subprocess exit code" );
}


template<> Description Describe< Benchmark::fork >()
{
    Description desc;
    desc.measures = "fork().";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::fork >()
{
    std::function< Durations( int ) > timer = []( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                TimePoints start_times = Start();
                pid_t pid = fork();
                durs += End( start_times );

                if (pid > 0) WaitPid( pid );
                else if (pid < 0) throw_system_error( errno, "fork()" );
                else quick_exit( 0 );
            }

            return durs;
        };

    return { timer, MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::fork_waitpid >()
{
    Description desc;
    desc.measures = "fork(), followed by waitpid().";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::fork_waitpid >()
{
    std::function< void() > f = []()
        {
            pid_t pid = fork();
            if (pid > 0) WaitPid( pid );
            else if (pid < 0) throw_system_error( errno, "fork()" );
            else quick_exit( 0 );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


} // namespace bench

