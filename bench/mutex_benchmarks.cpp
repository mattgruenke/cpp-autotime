////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements mutex-category benchmarks.
/*! @file

    These tests use the pthreads API, directly.  This sits at the bottom of std::mutex,
    but its functions are more resistent to being optimized-out by the compiler.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <future>
#include <thread>

#include <pthread.h>

#include "autotime/os.hpp"
#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "list.hpp"
#include "thread_utils.hpp"


using namespace autotime;

namespace bench
{


static void CreateDestroy()
{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

        // This call isn't strictly necessary, but it shifts some work into the
        //  first call to pthread_mutex_lock(), if not done.  Either way, the
        //  sum is the same.  Since the Lock/Unlock test is meant to isolate
        //  the cost of locking & unlocking a mutex (and not just on first-use),
        //  we go ahead and call this here.
    pthread_mutex_init( &mutex, nullptr );
    pthread_mutex_destroy( &mutex );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::mutex_create_destroy >()
{
    InitLibpthread();

    return { MakeTimer( &CreateDestroy ), MakeTimer( &Overhead_void<> ) };
}


static void CreateDestroyLockUnlock()
{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init( &mutex, nullptr );
    pthread_mutex_lock( &mutex );
    pthread_mutex_unlock( &mutex );
    pthread_mutex_destroy( &mutex );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::mutex_lock_release >()
{
    InitLibpthread();

    return { MakeTimer( &CreateDestroyLockUnlock ), MakeTimer( &CreateDestroy ) };
}


    // This object starts another thread to hold the mutex, since try-locking
    //  a self-owned mutex is not the normal scenario for lock contention and
    //  likely performs differently.
struct LockedMutex
{
    pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
    std::promise< void > stop_promise_;
    std::thread thread_;

    LockedMutex()
    {
        InitLibpthread();

        pthread_mutex_init( &mutex_, nullptr );

        // Start the thread and wait until its initialization is complete.
        std::promise< void > started_promise;
        std::future< void > started_future = started_promise.get_future();
        thread_ = std::thread( &LockedMutex::threadfunc, this, std::move( started_promise ) );
        started_future.get();
    }

    ~LockedMutex()
    {
        // Signal the thread to exit.
        stop_promise_.set_value();
        thread_.join();

        pthread_mutex_destroy( &mutex_ );
    }

    void threadfunc( std::promise< void > started_promise )
    {
        SetCoreAffinity( GetSecondaryCoreId() );

        // Use the future to receive a notification that it's time to stop.
        std::future< void > stop_future = stop_promise_.get_future();

        // Lock the mutex and signal the constructor that the class is ready to use.
        pthread_mutex_lock( &mutex_ );
        started_promise.set_value();

        // Wait until notified to stop.
        stop_future.get();
        pthread_mutex_unlock( &mutex_ );
    }
};


template<> autotime::BenchTimers MakeTimers< Benchmark::mutex_trylock >()
{
    static LockedMutex locked;

    int (*f)() = []()
        {
            return pthread_mutex_trylock( &locked.mutex_ );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


} // namespace bench

