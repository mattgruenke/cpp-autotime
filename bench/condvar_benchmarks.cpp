////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements condvar-category benchmarks.
/*! @file

    These tests use the pthreads API, directly.  This sits at the bottom of
    std::condition_variable, but its functions are more resistent to being
    optimized-out by the compiler.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <future>
#include <thread>

#include <pthread.h>

#include "autotime/os.hpp"
#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "error_utils.hpp"
#include "list.hpp"
#include "thread_utils.hpp"


using namespace autotime;

namespace bench
{


static void CreateDestroy()
{
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

        // This call isn't strictly necessary, but it shifts some work into the
        //  first call to pthread_cond_signal(), if not done.  Either way, the
        //  sum is the same.  Since the Signal test is meant to isolate the
        //  cost of signalling a condition variable (and not just on first-use),
        //  we go ahead and call this here.
    pthread_cond_init( &cond, nullptr );
    pthread_cond_destroy( &cond );
}


struct Cond
{
    pthread_cond_t var_ = PTHREAD_COND_INITIALIZER;

    Cond()
    {
        InitLibpthread();

        pthread_cond_init( &var_, nullptr );
    }

    ~Cond()
    {
        pthread_cond_destroy( &var_ );
    }
};


template<> autotime::BenchTimers MakeTimers< Benchmark::condvar_broadcast >()
{
    static Cond cond;

    void (*f)() = []()
        {
            pthread_cond_broadcast( &cond.var_ );
        };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::condvar_create_destroy >()
{
    InitLibpthread();

    return { MakeTimer( &CreateDestroy ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::condvar_signal >()
{
    static Cond cond;

    void (*f)() = []()
        {
            pthread_cond_signal( &cond.var_ );
        };

    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


    // This object starts another thread to watch the condition variable, since
    //  signalling one likely performs differently based on whether it's being
    //  watched.
struct WatchedCond
{
    Cond cond_;
    pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
    bool stop_ = false;
    std::thread thread_;

    WatchedCond()
    {
        pthread_mutex_init( &mutex_, nullptr );

        // Start the thread and wait until its initialization is complete.
        std::promise< void > started_promise;
        std::future< void > started_future = started_promise.get_future();
        thread_ = std::thread( &WatchedCond::threadfunc, this, std::move( started_promise ) );
        started_future.get();
    }

    ~WatchedCond()
    {
        stop_ = true;
        pthread_cond_signal( &cond_.var_ );
        thread_.join();

        pthread_mutex_destroy( &mutex_ );
    }

    void threadfunc( std::promise< void > started_promise )
    {
        SetCoreAffinity( GetSecondaryCoreId() );

        started_promise.set_value();

        pthread_mutex_lock( &mutex_ );
        while (!stop_) pthread_cond_wait( &cond_.var_, &mutex_ );
        pthread_mutex_unlock( &mutex_ );
    }
};


template<> autotime::BenchTimers MakeTimers< Benchmark::condvar_signal_watched >()
{
    std::shared_ptr< WatchedCond > p_watched{ new WatchedCond() };

    std::function< void() > f = [p_watched]()
        {
            // Normally, you'd get the mutex, but that could force a context switch.
            //  We're trying to isolate just the signalling time.
            int ev = pthread_cond_signal( &p_watched->cond_.var_ );
            if (ev) throw_system_error( ev, "pthread_cond_signal()" );
        };

    std::function< void() > o = [p_watched]() {};

    return { MakeTimer( f ), MakeTimer( o ) };
}


} // namespace bench

