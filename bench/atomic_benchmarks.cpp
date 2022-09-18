////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements atomic-category benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <future>
#include <thread>

#include "autotime/os.hpp"
#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "list.hpp"
#include "thread_utils.hpp"


using namespace autotime;

namespace bench
{


template<> autotime::BenchTimers MakeTimers< Benchmark::atomic_increment >()
{
    static std::atomic< int > i{ 0 };

    void (*f)() = []() { ++i; };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::atomic_cmp_exchg_weak_xpctd >()
{
    static std::atomic< int > i{ 42 };

    void (*f)() = []()
        {
            int expected = 42;
            i.compare_exchange_weak( expected, 42 );
        };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::atomic_cmp_exchg_weak_other >()
{
    static std::atomic< int > i{ 777 };

    void (*f)() = []()
        {
            int expected = 42;
            i.compare_exchange_weak( expected, 42 );
        };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::atomic_cmp_exchg_strong_xpctd >()
{
    static std::atomic< int > i{ 42 };

    void (*f)() = []()
        {
            int expected = 42;
            i.compare_exchange_strong( expected, 42 );
        };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::atomic_cmp_exchg_strong_other >()
{
    static std::atomic< int > i{ 777 };

    void (*f)() = []()
        {
            int expected = 42;
            i.compare_exchange_strong( expected, 42 );
        };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


static Durations PingPong( int num_iters )
{
    static std::atomic< int > i;
    i = 0;

    std::promise< void > started_promise;
    std::future< void > started_future = started_promise.get_future();
    std::atomic< bool > stop{ false };
    std::thread thread( [&stop, &started_promise]()
        {
            SetCoreAffinity( GetSecondaryCoreId() );

            started_promise.set_value();

            int exp = 0;
            int counter = 0;    // counter avoids sampling stop too often.
            while ((counter++ % 256) || !stop)
            {
                int x = exp;
                if (i.compare_exchange_weak( x, exp + 1 )) exp += 2;
            }
        } );

    started_future.get();

    static int exp;
    exp = 1;

    // Since the thread started, the value will go to 1.
    //  We want to start timing right after incrementing it again.
    int x = exp;
    while (!i.compare_exchange_weak( x, exp + 1 )) x = exp;
    exp += 2;

    void (*f)() = []()
        {
            int x = exp;
            while (!i.compare_exchange_weak( x, exp + 1 )) x = exp;
            exp += 2;
        };

    Durations durs = Time( f, num_iters );

    stop = true;
    thread.join();

    return durs;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::atomic_pingpong >()
{
    return { &PingPong,  MakeTimer( &Overhead_void<> ) };
}


} // namespace bench

