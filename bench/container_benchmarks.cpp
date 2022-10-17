////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements benchmarks of various containers.
/*! @file

    TO_DO: deque, hashset

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "list.hpp"


using namespace autotime;

namespace bench
{


////////////////////////////////
// Test data generation:
////////////////////////////////

template< typename elem_t > static elem_t MakeElement( size_t i )
{
    return static_cast< elem_t >( i );
}


template<> std::string MakeElement< std::string >( size_t i )
{
    static std::vector< std::string > words = []()
        {
            std::vector< std::string > result;
            std::ifstream file( "/usr/share/dict/words" );  // TO_DO: make this a CLI parameter.
            std::string word;
            while (std::getline( file,word )) result.push_back( word );
            return result;
        }();

    return words.at( i );
}


template< typename elem_t > static bool CheckElement( const elem_t &val )
{
    return val >= 0;
}


template<> bool CheckElement< std::string >( const std::string &val )
{
    return !val.empty();
}


template< typename elem_t > std::shared_ptr< elem_t[] > MakeData( size_t n )
{
    srandom( 1 );   // TO_DO: make this a commandline parameter.

#if 0
    std::multimap< int, elem_t > shuffled_vals;
    for (size_t i = 0; i < n; ++i)
    {
        shuffled_vals.insert( { static_cast< int >( random() ), MakeElement< elem_t >( i ) } );
    }

#else   // Both work, but below is marginally faster than above.

    std::vector< std::pair< int, elem_t > > shuffled_vals;
    for (size_t i = 0; i < n; ++i)
    {
        shuffled_vals.push_back( { static_cast< int >( random() ), MakeElement< elem_t >( i ) } );
    }
    std::sort( shuffled_vals.begin(), shuffled_vals.end() );
#endif

    std::shared_ptr< elem_t[] > result{ new elem_t[n] };
    auto shuffled_iter = shuffled_vals.begin();
    for (size_t i = 0; i < n; ++i) result[i] = (shuffled_iter++)->second;

    return result;
}


////////////////////////////////
// Dummy containers:
////////////////////////////////

template< typename container_t > container_t &Writable();


////////////////////////////////
// Insert generics:
////////////////////////////////

template< typename element_t >
    inline void InsertElement( std::set< element_t > &set, element_t value )
{
    set.insert( value );
}


template< typename element_t >
    inline void InsertElement( std::list< element_t > &l, element_t value )
{
    l.push_back( value );
}


template< typename element_t >
    inline void InsertElement( std::vector< element_t > &v, element_t value )
{
    v.push_back( value );
}


template< typename container_t >
    container_t Insert( const typename container_t::value_type *data, size_t n )
{
    container_t result;

#if 1
    using element_t = typename container_t::value_type;
    for (size_t i = 0; i < n; ++i) InsertElement< element_t >( result, data[i] );
#else
    for (size_t i = 0; i < n; ++i) result.insert( data[i] );
#endif

    return result;
}


template< typename container_t >
    static Durations InsertTimer(
        std::shared_ptr< typename container_t::value_type[] > data, size_t n, int num_iters )
{
    Durations durations{};
    for (int count = 0; count < num_iters; ++count)
    {
        TimePoints start_times = Start();
        container_t container = Insert< container_t >( data.get(), n );
        durations += End( start_times );
    }

    return durations;
}


template< typename container_t >
    static autotime::BenchTimers MakeInsertTimers( size_t n )
{
    using namespace std::placeholders;
    using value_t = typename container_t::value_type;
    std::shared_ptr< value_t[] > data = MakeData< value_t >( n );
    return { std::bind( &InsertTimer< container_t >, data, n, _1 ), nullptr };
}


////////////////////////////////
// Counting generics:
////////////////////////////////

template< typename container_t >
    static size_t Count( const container_t &c )
{
    size_t count = 0;
    for (const auto &e: c) if (CheckElement< typename container_t::value_type >( e )) ++count;
    return count;
}


size_t CountResult = 0;


template< typename container_t >
    static Durations CountTimer( std::shared_ptr< container_t > container, int num_iters )
{
    const container_t &c = *container;
    std::function< void() > f = [c]()
        {
            CountResult = Count< container_t >( c );
        };

    return Time( f, num_iters );
}


bool ContainerEmpty = false;


template< typename container_t >
    static Durations ContainerOverhead( std::shared_ptr< container_t > p, int num_iters )
{
    const container_t &c = *p;
    std::function< void() > f = [c]()
        {
            ContainerEmpty = c.empty();
        };

    return Time( f, num_iters );
}


template< typename container_t >
    static autotime::BenchTimers MakeCountTimers( size_t n )
{
    using namespace std::placeholders;
    using value_t = typename container_t::value_type;
    std::shared_ptr< value_t[] > data = MakeData< value_t >( n );
    std::shared_ptr< container_t > container{
        new container_t{ Insert< container_t >( data.get(), n ) } };

    return {
            std::bind( &CountTimer< container_t >, container, _1 ),
            std::bind( &ContainerOverhead< container_t >, container, _1 )
        };
}


////////////////////////////////
// Find generics:
////////////////////////////////

template< typename element_t >
    inline bool HasElement( const std::set< element_t > &set, element_t value )
{
    return set.find( value ) != set.end();
}


template< typename element_t >
    inline bool HasElement( const std::list< element_t > &l, element_t value )
{
    return std::find( l.begin(), l.end(), value ) != l.end();
}


template< typename element_t >
    inline bool HasElement( const std::vector< element_t > &v, element_t value )
{
    return std::find( v.begin(), v.end(), value ) != v.end();
}


template< typename container_t >
    static Durations FindTimer(
        std::shared_ptr< container_t > container,
        std::shared_ptr< typename container_t::value_type[] > data,
        int data_size,
        int num_iters )
{
    const container_t &c = *container;

    unsigned int count = 0;
    TimePoints start_times = Start();
    for (int i = 0; i < num_iters; ++i)
    {
        using element_t = typename container_t::value_type;
        if (HasElement< element_t >( c, data[i % data_size] )) ++count;
    }
    CountResult = count;

    return End( start_times );
}


template< typename container_t >
    static autotime::BenchTimers MakeFindTimers( bool sort, size_t n )
{
    using namespace std::placeholders;
    using value_t = typename container_t::value_type;
    std::shared_ptr< value_t[] > data = MakeData< value_t >( n );
    std::shared_ptr< container_t > container{
        new container_t{ Insert< container_t >( data.get(), n ) } };

    if (sort) std::sort( data.get(), data.get() + n );

    return { std::bind( &FindTimer< container_t >, container, data, n, _1 ), nullptr };
}


////////////////////////////////
// Copy generics:
////////////////////////////////

template< typename container_t >
    static Durations CopyTimer( std::shared_ptr< container_t > container, int num_iters )
{
    const container_t &src = *container;
    container_t &dst = Writable< container_t >();

    std::function< Durations() > f = [src, &dst]()
        {
            container_t tmp;
            tmp.swap( dst );

            TimePoints start_times = Start();
            dst = src;
            return End( start_times );
        };

    return Time( f, num_iters );
}


template< typename container_t >
    static Durations CopyOverhead( std::shared_ptr< container_t > container, int num_iters )
{
    const container_t &src = *container;
    container_t &dst = Writable< container_t >();
    dst.clear();

    std::function< void() > f = [src, dst]()
        {
            if (src.empty() || !dst.empty()) std::terminate();
        };

    return Time( f, num_iters );
}


template< typename container_t >
    static autotime::BenchTimers MakeCopyTimers( size_t n )
{
    using namespace std::placeholders;
    using value_t = typename container_t::value_type;
    std::shared_ptr< value_t[] > data = MakeData< value_t >( n );
    std::shared_ptr< container_t > container{
        new container_t{ Insert< container_t >( data.get(), n ) } };

    return {
            std::bind( &CopyTimer< container_t >, container, _1 ),
            std::bind( &CopyOverhead< container_t >, container, _1 )
        };
}


////////////////////////////////
// Destroy generics:
////////////////////////////////

template< typename container_t >
    static Durations DestroyTimer( std::shared_ptr< container_t > container, int num_iters )
{
    const container_t &src = *container;
    container_t &dst = Writable< container_t >();

    std::function< Durations() > f = [src, &dst]()
        {
            dst = src;

            TimePoints start_times = Start();
            {
                container_t tmp;
                tmp.swap( dst );
            }
            return End( start_times );
        };

    return Time( f, num_iters );
}


template< typename container_t >
    static autotime::BenchTimers MakeDestroyTimers( size_t n )
{
    using namespace std::placeholders;
    using value_t = typename container_t::value_type;
    std::shared_ptr< value_t[] > data = MakeData< value_t >( n );
    std::shared_ptr< container_t > container{
        new container_t{ Insert< container_t >( data.get(), n ) } };

    return {
            std::bind( &DestroyTimer< container_t >, container, _1 ),
            std::bind( &CopyOverhead< container_t >, container, _1 )
        };
}


////////////////////////////////////////////////////////////
// Category::std_list:
////////////////////////////////////////////////////////////

std::list< int32_t > List_int32;
std::list< int64_t > List_int64;
std::list< float > List_float;
std::list< double > List_double;
std::list< std::string > List_string;


template<> std::list< int32_t > &Writable< std::list< int32_t > >()
{
    return List_int32;
}


template<> std::list< int64_t > &Writable< std::list< int64_t > >()
{
    return List_int64;
}


template<> std::list< float > &Writable< std::list< float > >()
{
    return List_float;
}


template<> std::list< double > &Writable< std::list< double > >()
{
    return List_double;
}


template<> std::list< std::string > &Writable< std::list< std::string > >()
{
    return List_string;
}

template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_insert16 >()
{
    return MakeInsertTimers< std::list< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_insert256 >()
{
    return MakeInsertTimers< std::list< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_insert4k >()
{
    return MakeInsertTimers< std::list< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_insert64k >()
{
    return MakeInsertTimers< std::list< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_insert1M >()
{
    return MakeInsertTimers< std::list< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int64_insert4k >()
{
    return MakeInsertTimers< std::list< int64_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_float_insert4k >()
{
    return MakeInsertTimers< std::list< float > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_double_insert4k >()
{
    return MakeInsertTimers< std::list< double > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_string_insert4k >()
{
    return MakeInsertTimers< std::list< std::string > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_iterate16 >()
{
    return MakeCountTimers< std::list< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_iterate256 >()
{
    return MakeCountTimers< std::list< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_iterate4k >()
{
    return MakeCountTimers< std::list< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_iterate64k >()
{
    return MakeCountTimers< std::list< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_iterate1M >()
{
    return MakeCountTimers< std::list< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int64_iterate64k >()
{
    return MakeCountTimers< std::list< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_float_iterate64k >()
{
    return MakeCountTimers< std::list< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_double_iterate64k >()
{
    return MakeCountTimers< std::list< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_string_iterate64k >()
{
    return MakeCountTimers< std::list< std::string > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_find1 >()
{
    return MakeFindTimers< std::list< int32_t > >( true, 1 << 0 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_find16 >()
{
    return MakeFindTimers< std::list< int32_t > >( true, 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_find256 >()
{
    return MakeFindTimers< std::list< int32_t > >( true, 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_find4k >()
{
    return MakeFindTimers< std::list< int32_t > >( true, 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_find64k >()
{
    return MakeFindTimers< std::list< int32_t > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_find1M >()
{
    return MakeFindTimers< std::list< int32_t > >( true, 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int64_find64k >()
{
    return MakeFindTimers< std::list< int64_t > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_float_find64k >()
{
    return MakeFindTimers< std::list< float > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_double_find64k >()
{
    return MakeFindTimers< std::list< double > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_string_find64k >()
{
    return MakeFindTimers< std::list< std::string > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_copy16 >()
{
    return MakeCopyTimers< std::list< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_copy256 >()
{
    return MakeCopyTimers< std::list< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_copy4k >()
{
    return MakeCopyTimers< std::list< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_copy64k >()
{
    return MakeCopyTimers< std::list< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_copy1M >()
{
    return MakeCopyTimers< std::list< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int64_copy64k >()
{
    return MakeCopyTimers< std::list< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_float_copy64k >()
{
    return MakeCopyTimers< std::list< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_double_copy64k >()
{
    return MakeCopyTimers< std::list< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_string_copy64k >()
{
    return MakeCopyTimers< std::list< std::string > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_destroy16 >()
{
    return MakeDestroyTimers< std::list< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_destroy256 >()
{
    return MakeDestroyTimers< std::list< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_destroy4k >()
{
    return MakeDestroyTimers< std::list< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_destroy64k >()
{
    return MakeDestroyTimers< std::list< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int32_destroy1M >()
{
    return MakeDestroyTimers< std::list< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_int64_destroy64k >()
{
    return MakeDestroyTimers< std::list< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_float_destroy64k >()
{
    return MakeDestroyTimers< std::list< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_double_destroy64k >()
{
    return MakeDestroyTimers< std::list< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::list_string_destroy64k >()
{
    return MakeDestroyTimers< std::list< std::string > >( 1 << 16 );
}




////////////////////////////////////////////////////////////
// Category::std_set:
////////////////////////////////////////////////////////////

std::set< int32_t > Set_int32;
std::set< int64_t > Set_int64;
std::set< float > Set_float;
std::set< double > Set_double;
std::set< std::string > Set_string;


template<> std::set< int32_t > &Writable< std::set< int32_t > >()
{
    return Set_int32;
}


template<> std::set< int64_t > &Writable< std::set< int64_t > >()
{
    return Set_int64;
}


template<> std::set< float > &Writable< std::set< float > >()
{
    return Set_float;
}


template<> std::set< double > &Writable< std::set< double > >()
{
    return Set_double;
}


template<> std::set< std::string > &Writable< std::set< std::string > >()
{
    return Set_string;
}

template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_insert16 >()
{
    return MakeInsertTimers< std::set< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_insert256 >()
{
    return MakeInsertTimers< std::set< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_insert4k >()
{
    return MakeInsertTimers< std::set< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_insert64k >()
{
    return MakeInsertTimers< std::set< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_insert1M >()
{
    return MakeInsertTimers< std::set< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int64_insert4k >()
{
    return MakeInsertTimers< std::set< int64_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_float_insert4k >()
{
    return MakeInsertTimers< std::set< float > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_double_insert4k >()
{
    return MakeInsertTimers< std::set< double > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_string_insert4k >()
{
    return MakeInsertTimers< std::set< std::string > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_iterate16 >()
{
    return MakeCountTimers< std::set< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_iterate256 >()
{
    return MakeCountTimers< std::set< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_iterate4k >()
{
    return MakeCountTimers< std::set< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_iterate64k >()
{
    return MakeCountTimers< std::set< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_iterate1M >()
{
    return MakeCountTimers< std::set< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int64_iterate64k >()
{
    return MakeCountTimers< std::set< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_float_iterate64k >()
{
    return MakeCountTimers< std::set< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_double_iterate64k >()
{
    return MakeCountTimers< std::set< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_string_iterate64k >()
{
    return MakeCountTimers< std::set< std::string > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_find1 >()
{
    return MakeFindTimers< std::set< int32_t > >( false, 1 << 0 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_find16 >()
{
    return MakeFindTimers< std::set< int32_t > >( false, 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_find256 >()
{
    return MakeFindTimers< std::set< int32_t > >( false, 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_find4k >()
{
    return MakeFindTimers< std::set< int32_t > >( false, 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_find64k >()
{
    return MakeFindTimers< std::set< int32_t > >( false, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_find1M >()
{
    return MakeFindTimers< std::set< int32_t > >( false, 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int64_find64k >()
{
    return MakeFindTimers< std::set< int64_t > >( false, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_float_find64k >()
{
    return MakeFindTimers< std::set< float > >( false, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_double_find64k >()
{
    return MakeFindTimers< std::set< double > >( false, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_string_find64k >()
{
    return MakeFindTimers< std::set< std::string > >( false, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_copy16 >()
{
    return MakeCopyTimers< std::set< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_copy256 >()
{
    return MakeCopyTimers< std::set< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_copy4k >()
{
    return MakeCopyTimers< std::set< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_copy64k >()
{
    return MakeCopyTimers< std::set< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_copy1M >()
{
    return MakeCopyTimers< std::set< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int64_copy64k >()
{
    return MakeCopyTimers< std::set< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_float_copy64k >()
{
    return MakeCopyTimers< std::set< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_double_copy64k >()
{
    return MakeCopyTimers< std::set< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_string_copy64k >()
{
    return MakeCopyTimers< std::set< std::string > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_destroy16 >()
{
    return MakeDestroyTimers< std::set< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_destroy256 >()
{
    return MakeDestroyTimers< std::set< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_destroy4k >()
{
    return MakeDestroyTimers< std::set< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_destroy64k >()
{
    return MakeDestroyTimers< std::set< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int32_destroy1M >()
{
    return MakeDestroyTimers< std::set< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_int64_destroy64k >()
{
    return MakeDestroyTimers< std::set< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_float_destroy64k >()
{
    return MakeDestroyTimers< std::set< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_double_destroy64k >()
{
    return MakeDestroyTimers< std::set< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::set_string_destroy64k >()
{
    return MakeDestroyTimers< std::set< std::string > >( 1 << 16 );
}


////////////////////////////////////////////////////////////
// Category::std_vector:
////////////////////////////////////////////////////////////

std::vector< int32_t > Vector_int32;
std::vector< int64_t > Vector_int64;
std::vector< float > Vector_float;
std::vector< double > Vector_double;
std::vector< std::string > Vector_string;


template<> std::vector< int32_t > &Writable< std::vector< int32_t > >()
{
    return Vector_int32;
}


template<> std::vector< int64_t > &Writable< std::vector< int64_t > >()
{
    return Vector_int64;
}


template<> std::vector< float > &Writable< std::vector< float > >()
{
    return Vector_float;
}


template<> std::vector< double > &Writable< std::vector< double > >()
{
    return Vector_double;
}


template<> std::vector< std::string > &Writable< std::vector< std::string > >()
{
    return Vector_string;
}

template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_insert16 >()
{
    return MakeInsertTimers< std::vector< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_insert256 >()
{
    return MakeInsertTimers< std::vector< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_insert4k >()
{
    return MakeInsertTimers< std::vector< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_insert64k >()
{
    return MakeInsertTimers< std::vector< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_insert1M >()
{
    return MakeInsertTimers< std::vector< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int64_insert4k >()
{
    return MakeInsertTimers< std::vector< int64_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_float_insert4k >()
{
    return MakeInsertTimers< std::vector< float > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_double_insert4k >()
{
    return MakeInsertTimers< std::vector< double > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_string_insert4k >()
{
    return MakeInsertTimers< std::vector< std::string > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_iterate16 >()
{
    return MakeCountTimers< std::vector< int32_t > >( 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_iterate256 >()
{
    return MakeCountTimers< std::vector< int32_t > >( 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_iterate4k >()
{
    return MakeCountTimers< std::vector< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_iterate64k >()
{
    return MakeCountTimers< std::vector< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_iterate1M >()
{
    return MakeCountTimers< std::vector< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int64_iterate64k >()
{
    return MakeCountTimers< std::vector< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_float_iterate64k >()
{
    return MakeCountTimers< std::vector< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_double_iterate64k >()
{
    return MakeCountTimers< std::vector< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_string_iterate64k >()
{
    return MakeCountTimers< std::vector< std::string > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_find1 >()
{
    return MakeFindTimers< std::vector< int32_t > >( true, 1 << 0 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_find16 >()
{
    return MakeFindTimers< std::vector< int32_t > >( true, 1 << 4 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_find256 >()
{
    return MakeFindTimers< std::vector< int32_t > >( true, 1 << 8 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_find4k >()
{
    return MakeFindTimers< std::vector< int32_t > >( true, 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_find64k >()
{
    return MakeFindTimers< std::vector< int32_t > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_find1M >()
{
    return MakeFindTimers< std::vector< int32_t > >( true, 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int64_find64k >()
{
    return MakeFindTimers< std::vector< int64_t > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_float_find64k >()
{
    return MakeFindTimers< std::vector< float > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_double_find64k >()
{
    return MakeFindTimers< std::vector< double > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_string_find64k >()
{
    return MakeFindTimers< std::vector< std::string > >( true, 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_copy4k >()
{
    return MakeCopyTimers< std::vector< int32_t > >( 1 << 12 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_copy64k >()
{
    return MakeCopyTimers< std::vector< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_copy1M >()
{
    return MakeCopyTimers< std::vector< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int64_copy64k >()
{
    return MakeCopyTimers< std::vector< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_float_copy64k >()
{
    return MakeCopyTimers< std::vector< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_double_copy64k >()
{
    return MakeCopyTimers< std::vector< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_string_copy64k >()
{
    return MakeCopyTimers< std::vector< std::string > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_destroy64k >()
{
    return MakeDestroyTimers< std::vector< int32_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int32_destroy1M >()
{
    return MakeDestroyTimers< std::vector< int32_t > >( 1 << 20 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_int64_destroy64k >()
{
    return MakeDestroyTimers< std::vector< int64_t > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_float_destroy64k >()
{
    return MakeDestroyTimers< std::vector< float > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_double_destroy64k >()
{
    return MakeDestroyTimers< std::vector< double > >( 1 << 16 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::vec_string_destroy64k >()
{
    return MakeDestroyTimers< std::vector< std::string > >( 1 << 16 );
}


} // namespace bench

