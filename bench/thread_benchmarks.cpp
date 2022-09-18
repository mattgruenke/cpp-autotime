////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements thread-category benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

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


template<> autotime::BenchTimers MakeTimers< Benchmark::thread_create_destroy >()
{
    void (*f)() = []()
        {
            std::thread thread( [](){} );
            thread.join();
        };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


    // Models synchronous communication with a peer thread.
struct Peer
{
    bool busy_ = false;
    bool stop_ = false;
    std::mutex mutex_;
    std::condition_variable busy_condition_;
    std::condition_variable ready_condition_;
    std::thread thread_;

    Peer()
    {
        std::promise< void > started_promise;
        std::future< void > started_future = started_promise.get_future();
        thread_ = std::thread( &Peer::threadfunc, this, std::move( started_promise ) );
        started_future.get();
    }

    ~Peer()
    {
        std::unique_lock< std::mutex > lock{ mutex_ };
        stop_ = true;
        busy_condition_.notify_one();
        lock.unlock();

        thread_.join();
    }

    void threadfunc( std::promise< void > started_promise )
    {
        SetCoreAffinity( GetSecondaryCoreId() );

        started_promise.set_value();

        while (!stop_)
        {
            std::unique_lock< std::mutex > lock{ mutex_ };
            while (!busy_ && !stop_) busy_condition_.wait( lock );

            busy_ = false;
            ready_condition_.notify_one();
        }
    }

    void send()
    {
        std::unique_lock< std::mutex > lock{ mutex_ };
        while (busy_) ready_condition_.wait( lock );

        busy_ = true;
        busy_condition_.notify_one();
    }
};


template<> autotime::BenchTimers MakeTimers< Benchmark::thread_pingpong >()
{
    std::shared_ptr< Peer > p_peer{ new Peer() };
    std::function< void() > f = [p_peer]() { p_peer->send(); };
    std::function< void() > o = [p_peer]() {};
    return { MakeTimer( f ), MakeTimer( o ) };
}


} // namespace bench

