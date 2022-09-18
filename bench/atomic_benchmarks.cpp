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
#include <memory>
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


    // Owns an atomic_int and a thread which increments it when its value is even.
struct CounterParty
{
    std::atomic< int > i_;
    std::atomic< bool > stop_;
    std::thread thread_;

    CounterParty()
    :
        i_{ 0 },
        stop_{ false }
    {
        std::promise< void > started_promise;
        std::future< void > started_future = started_promise.get_future();
        thread_ = std::thread( &CounterParty::threadfunc, this, std::move( started_promise ) );
        started_future.get();
    }

    ~CounterParty()
    {
        stop_ = true;
        thread_.join();
    }

    void threadfunc( std::promise< void > started_promise )
    {
        SetCoreAffinity( GetSecondaryCoreId() );

        started_promise.set_value();

        int exp = 0;
        int counter = 0;    // counter avoids sampling stop too often.
        while ((counter++ % 256) || !stop_)
        {
            int x = exp;
            if (i_.compare_exchange_weak( x, exp + 1 )) exp += 2;
        }
    }
};


static Durations PingPong( int num_iters )
{
    std::shared_ptr< CounterParty > p_counterparty{ new CounterParty() };
    std::atomic< int > &i = p_counterparty->i_;

    int exp = i;
    if (!(exp % 2)) exp += 1;   // Avoid race by ensuring exp is odd.

    //  We want to start timing right after incrementing the counter again.
    int x = exp;
    while (!i.compare_exchange_weak( x, exp + 1 )) x = exp;
    exp += 2;

    std::function< void() > f = [&i, &exp]()
        {
            int x = exp;
            while (!i.compare_exchange_weak( x, exp + 1 )) x = exp;
            exp += 2;
        };

    Durations durs = Time( f, num_iters );

    return durs;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::atomic_pingpong >()
{
    return { &PingPong,  MakeTimer( &Overhead_void<> ) };
}


} // namespace bench

