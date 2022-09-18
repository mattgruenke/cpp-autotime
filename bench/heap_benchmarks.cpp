////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements heap-category benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>

#include <malloc.h>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "list.hpp"


using namespace autotime;

namespace bench
{


template<
    size_t size
>
static void new_delete()
{
#if 0
    std::unique_ptr< uint8_t[] > p{ new uint8_t[size] };
#else
    uint8_t *p = new uint8_t[size];
    delete [] p;
#endif
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_16 >()
{
    return { MakeTimer( &new_delete< 1 << 4 > ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_256 >()
{
    return { MakeTimer( &new_delete< 1 << 8 > ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_4k >()
{
    return { MakeTimer( &new_delete< 1 << 12 > ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_64k >()
{
    return { MakeTimer( &new_delete< 1 << 16 > ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_1M >()
{
    return { MakeTimer( &new_delete< 1 << 20 > ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_16M >()
{
    return { MakeTimer( &new_delete< 1 << 24 > ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_256M >()
{
    return { MakeTimer( &new_delete< 1 << 28 > ), MakeTimer( &Overhead_void<> ) };
}


static void reclaim_heap()
{
    // Returns heap memory to the OS.
    malloc_trim( 0 );
}


template<
    size_t size
>
static void new_delete_cold()
{
    reclaim_heap();
    new_delete< size >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_cold_16 >()
{
    return { MakeTimer( &new_delete_cold< 1 << 4 > ), MakeTimer( &reclaim_heap ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_cold_4k >()
{
    return { MakeTimer( &new_delete_cold< 1 << 12 > ), MakeTimer( &reclaim_heap ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_cold_1M >()
{
    return { MakeTimer( &new_delete_cold< 1 << 20 > ), MakeTimer( &reclaim_heap ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::new_delete_cold_256M >()
{
    return { MakeTimer( &new_delete_cold< 1 << 28 > ), MakeTimer( &reclaim_heap ) };
}


} // namespace bench

