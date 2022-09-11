////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements memory-category benchmarks.
/*! @file

    These consist of basic read, write, and copy operations, at different sizes.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "dispatch.hpp"

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"
#include "autotime/work.hpp"

#include <cstring>
#include <vector>


using namespace autotime;

namespace bench
{


template<
    size_t size
>
autotime::Timer MakeMemCopy()
{
    return []( int num_iters )
        {
            const static std::vector< uint8_t > src( size );
            static std::vector< uint8_t > dst( size );

            std::function< void() > f = []()
                {
                    memcpy( dst.data(), src.data(), size );
                };

            return Time( f, num_iters );
        };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memcpy_256 >()
{
    return { MakeMemCopy< 1 << 8 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memcpy_4k >()
{
    return { MakeMemCopy< 1 << 12 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memcpy_64k >()
{
    return { MakeMemCopy< 1 << 16 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memcpy_1M >()
{
    return { MakeMemCopy< 1 << 20 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memcpy_16M >()
{
    return { MakeMemCopy< 1 << 24 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memcpy_256M >()
{
    return { MakeMemCopy< 1 << 28 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<
    size_t size
>
autotime::Timer MakeStrLen()
{
    return []( int num_iters )
        {
            const static std::vector< char > src = []()
                {
                    std::vector< char > v( size, '1' );
                    v.back() = '\0';
                    return v;
                }();

            std::function< size_t() > f = []()
                {
                    return strlen( src.data() );
                };

            return Time( f, num_iters );
        };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strlen_256 >()
{
    return { MakeStrLen< 1 << 8 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strlen_4k >()
{
    return { MakeStrLen< 1 << 12 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strlen_64k >()
{
    return { MakeStrLen< 1 << 16 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strlen_1M >()
{
    return { MakeStrLen< 1 << 20 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strlen_16M >()
{
    return { MakeStrLen< 1 << 24 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strlen_256M >()
{
    return { MakeStrLen< 1 << 28 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<
    size_t size
>
autotime::Timer MakeMemSet()
{
    return []( int num_iters )
        {
            static std::vector< uint8_t > dst( size );

            std::function< void() > f = []()
                {
                    memset( dst.data(), 0, size );
                };

            return Time( f, num_iters );
        };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memset_256 >()
{
    return { MakeMemSet< 1 << 8 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memset_4k >()
{
    return { MakeMemSet< 1 << 12 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memset_64k >()
{
    return { MakeMemSet< 1 << 16 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memset_1M >()
{
    return { MakeMemSet< 1 << 20 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memset_16M >()
{
    return { MakeMemSet< 1 << 24 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memset_256M >()
{
    return { MakeMemSet< 1 << 28 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


} // namespace bench

