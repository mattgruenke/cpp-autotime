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

#include "autotime/os.hpp"
#include "autotime/overhead.hpp"
#include "autotime/time.hpp"
#include "autotime/work.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <future>
#include <type_traits>
#include <vector>

#include "description.hpp"
#include "thread_utils.hpp"


using namespace autotime;

namespace bench
{


template<
    typename t,
    size_t size
>
static std::array< t, size > MakeRandomArray()
{
    static_assert( (RAND_MAX & (RAND_MAX + 1LL)) == 0 ); // Ensure RAND_MAX + 1 is a power of 2.
    const int rand_bits = static_cast< int >( lrint( log2( RAND_MAX + 1.0 ) ) );
    constexpr int t_bits = sizeof( t ) * 8;
    constexpr t t_mask = static_cast< t >( (1LL << t_bits) - 1 );

    std::array< t, size > a;
    uint64_t r = 0;
    int n = 0;
    for (auto &e: a)
    {
        if (n < t_bits)
        {
            r |= (random() << n);
            n += rand_bits;
        }

        e = static_cast< t >( r & t_mask );
        r = r >> t_bits;
        n = n - t_bits;
    }

    return a;
}


static constexpr size_t RandomBlockSize = 4096;
static constexpr size_t RandomBlockMask = RandomBlockSize - 1;

template<
    typename t
>
using RandomBlock = std::array< t, RandomBlockSize >;


template<
    typename t
>
static const RandomBlock< t > &GetRandomBlock()
{
    static const RandomBlock< t > a = MakeRandomArray< t, RandomBlockSize >();
    return a;
}


static std::vector< uint8_t > MakeRandomVector( size_t size )
{
    const RandomBlock< uint8_t > &rand_block = GetRandomBlock< uint8_t >();
    std::vector< uint8_t > v( size );
    for (size_t i = 0; i < size; ++i) v[i] = rand_block[i & RandomBlockMask];

    return v;
}


template<
    size_t size
>
autotime::Timer MakeMemCopy()
{
    return []( int num_iters )
        {
            const static std::vector< uint8_t > src = MakeRandomVector( size );
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
autotime::Timer MakeStrCmp()
{
    return []( int num_iters )
        {
            auto make_vector = []()
                {
                    std::vector< char > v( size, '1' );
                    v.back() = '\0';
                    return v;
                };
            const static std::vector< char > a = make_vector();
            const static std::vector< char > b = make_vector();

            std::function< int() > f = []()
                {
                    return strcmp( a.data(), b.data() );
                };

            return Time( f, num_iters );
        };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strcmp_16 >()
{
    return { MakeStrCmp< 1 << 4 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strcmp_256 >()
{
    return { MakeStrCmp< 1 << 8 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strcmp_4k >()
{
    return { MakeStrCmp< 1 << 12 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strcmp_64k >()
{
    return { MakeStrCmp< 1 << 16 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strcmp_1M >()
{
    return { MakeStrCmp< 1 << 20 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strcmp_16M >()
{
    return { MakeStrCmp< 1 << 24 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strcmp_256M >()
{
    return { MakeStrCmp< 1 << 28 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<
    size_t size
>
autotime::Timer MakeStrNCpy()
{
    return []( int num_iters )
        {
            const static std::vector< char > src = []()
                {
                    std::vector< char > v( size, '1' );
                    v.back() = '\0';
                    return v;
                }();
            static std::vector< char > dst( size );

            std::function< void() > f = []()
                {
                    strncpy( dst.data(), src.data(), dst.size() );
                };

            return Time( f, num_iters );
        };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strncpy_16 >()
{
    return { MakeStrNCpy< 1 << 4 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strncpy_256 >()
{
    return { MakeStrNCpy< 1 << 8 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strncpy_4k >()
{
    return { MakeStrNCpy< 1 << 12 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strncpy_64k >()
{
    return { MakeStrNCpy< 1 << 16 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strncpy_1M >()
{
    return { MakeStrNCpy< 1 << 20 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strncpy_16M >()
{
    return { MakeStrNCpy< 1 << 24 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::strncpy_256M >()
{
    return { MakeStrNCpy< 1 << 28 >(), MakeTimer( MakeOverheadFn< void >() ) };
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
                    memset( dst.data(), 0xcc, size );
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


template<
    size_t size
>
autotime::Timer MakeMemRead()
{
    return []( int num_iters )
        {
#if 0
            using element_type = uint64_t;
#elif 1
            // g++ 7.5.0 implements this by emitting a movdqu instruction,
            //  which reads an unaligned "double quad word" (i.e. 128-bit) in one shot.

            // On Sandybridge, this is twice as fast as using uint64_t, in the 256-byte case.
            using element_type = std::array< uint64_t, 2 >;
#elif 0
            // Forces movaps - reads aligned "packed single" (i.e. 128-bit) in one shot.
            // On Sandbridge, the only way this seems to deliver a real gain is if the loop is
            //  unrolled by 2.  In that case, the gain is up to a further 2x over movdqu.
            using element_type = __m128;  // requires xmmintrin.h
#else
            // Forces movdqa - reads aligned "double quad-word" (i.e. 128-bit) in one shot.
            // On Sandybridge, this seems to perform about the same as movaps.
            //  Might be more energy-efficient, as it could avoid setting FP flags?
            using element_type = __v2du;  // requires emmintrin.h
#endif
            constexpr size_t n = size / sizeof( element_type );
            std::vector< element_type > src( n );
            volatile element_type *data = src.data();

            std::function< void() > f = [data]()
                {
                    // On Sandybridge, unrolling this by 4 can yield 3.2x and 2.1x speedups,
                    //  when using one of the aligned types (above), on the 256 and 4k cases.
                    //  Diminishing returns, beyond that.
                    for (size_t i = 0; i < n; ++i) data[i];
                };

            return Time( f, num_iters );
        };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memread_256 >()
{
    return { MakeMemRead< 1 << 8 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memread_4k >()
{
    return { MakeMemRead< 1 << 12 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memread_64k >()
{
    return { MakeMemRead< 1 << 16 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memread_1M >()
{
    return { MakeMemRead< 1 << 20 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memread_16M >()
{
    return { MakeMemRead< 1 << 24 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::memread_256M >()
{
    return { MakeMemRead< 1 << 28 >(), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::cache_false_sharing >()
{
    Description desc;
    desc.measures = "Performance impact of two threads touching the same cache line.";
    return desc;
}


struct FalselyShared
{
    uint64_t a = 0;
    uint64_t b = 0;
};


union AlignedFalselyShared
{
    FalselyShared fs;
    std::aligned_storage< 64, 64 >::type aligned;
};


autotime::Timer MakeFalseSharing()
{
    return []( int num_iters )
        {
            constexpr int n = 2;    // The more elements, the less impact is observed?
            AlignedFalselyShared array[n] = {};
            std::promise< void > started_promise;
            std::future< void > started_future = started_promise.get_future();
            volatile bool stop = false;

#if 0   // On Sandybridge, it doesn't seem to make much difference which is used (at n=128).

            // Shuffle the entries, in case it helps fool the core's prefetcher.
            std::vector< AlignedFalselyShared * > ptrs( n );
            for (size_t i = 0; i < n; ++i) ptrs[i] = &array[i];
            std::random_shuffle( ptrs.begin(), ptrs.end() );

            auto thread = std::thread( [&ptrs, num_iters, &started_promise, &stop]()
                    {
                        SetCoreAffinity( GetSecondaryCoreId() );
                        started_promise.set_value();
                        while (!stop) for (const auto p: ptrs) ++p->fs.b;
                    } );

            started_future.get();

            std::function< void() > f = [&ptrs]()
                {
                    for (const auto p: ptrs) ++p->fs.a;
                };

#else

            auto thread = std::thread( [&array, num_iters, &started_promise, &stop]()
                    {
                        SetCoreAffinity( GetSecondaryCoreId() );
                        started_promise.set_value();
                        while (!stop) for (AlignedFalselyShared &aligned: array) ++aligned.fs.b;
                    } );

            started_future.get();

            std::function< void() > f = [&array]()
                {
                    for (AlignedFalselyShared &aligned: array) ++aligned.fs.a;
                };

#endif

            Durations durs = Time( f, num_iters ) / n;
            stop = true;
            thread.join();

            return durs;
        };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::cache_false_sharing >()
{
    return { MakeFalseSharing(), MakeTimer( MakeOverheadFn< void >() ) };
}


} // namespace bench

