////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements benchmarks of hash functions.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <boost/mpl/min_max.hpp>
#include <boost/mpl/size_t.hpp>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "container_utils.hpp"
#include "description.hpp"
#include "dispatch.hpp"
#include "format_utils.hpp"
#include "list.hpp"


using namespace autotime;

namespace bench
{


template<> Description Describe< Category::std_hash >()
{
    Description desc;
    desc.measures = "std::hash<> computation over different numeric types and string lengths.";
    return desc;
}


static constexpr size_t MaxSize = 1 << 16;  // Could size this dynamically as (L2 / 4).

static std::vector< size_t > HashVector;


template< size_t size >
    static Durations MakeHashOverheadTimer( int n )
{
    size_t *dst = HashVector.data();
    int i = 0;
    std::function< void() > g = [dst, i]() mutable
        {
            dst[i] = i;
            i = (i + 1) & (size - 1);
        };

    return Time( g, n );
}


template< typename value_type >
    static autotime::BenchTimers MakeHashTimers()
{
    constexpr size_t size = MaxSize / sizeof( value_type );
    HashVector.resize( size );
    std::shared_ptr< value_type[] > data = MakeData< value_type >( size );

    std::function< Durations( int ) > f = [data]( int n )
        {
            const std::hash< value_type > hash{};
            const value_type *src = data.get();
            size_t *dst = HashVector.data();
            int i = 0;
            std::function< void() > g = [hash, src, dst, i]() mutable
                {
                    dst[i] = hash( src[i] );
                    i = (i + 1) & (size - 1);
                };

            return Time( g, n );
        };

    return { f, &MakeHashOverheadTimer< size > };
}


template< size_t value_len >
    static autotime::BenchTimers MakeStringHashTimers()
{
    namespace mpl = boost::mpl;

    constexpr size_t size_unbounded = 
        MaxSize / mpl::max< mpl::size_t< 16 >, mpl::size_t< value_len > >::type::value;
    constexpr size_t size =
        mpl::max< mpl::size_t< size_unbounded >, mpl::size_t< 2 > >::type::value;

    HashVector.resize( size );
    std::shared_ptr< std::string[] > data = MakeStringData( value_len, size );

    std::function< Durations( int ) > f = [data]( int n )
        {
            const std::hash< std::string > hash{};
            const std::string *src = data.get();
            size_t *dst = HashVector.data();
            int i = 0;
            std::function< void() > g = [hash, src, dst, i]() mutable
                {
                    dst[i] = hash( src[i] );
                    i = (i + 1) & (size - 1);
                };

            return Time( g, n );
        };

    return { f, &MakeHashOverheadTimer< size > };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_int8 >()
{
    return MakeHashTimers< int8_t >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_int16 >()
{
    return MakeHashTimers< int16_t >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_int32 >()
{
    return MakeHashTimers< int32_t >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_int64 >()
{
    return MakeHashTimers< int64_t >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_float >()
{
    return MakeHashTimers< float >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_double >()
{
    return MakeHashTimers< double >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_string_1 >()
{
    return MakeStringHashTimers< 1 << 0 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_string_16 >()
{
    return MakeStringHashTimers< 1 << 4 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_string_256 >()
{
    return MakeStringHashTimers< 1 << 8 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_string_4k >()
{
    return MakeStringHashTimers< 1 << 12 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::hash_string_64k >()
{
    return MakeStringHashTimers< 1 << 16 >();
}


template<
    typename type
>
static Description DescribeHash()
{

    std::ostringstream oss;
    oss << "std::hash< " << PrettyTypename< type >() << " > over multiple values.";

    Description desc;
    desc.measures = oss.str();

    return desc;
}


template<> Description Describe< Benchmark::hash_int8 >()
{
    return DescribeHash< int8_t >();
}


template<> Description Describe< Benchmark::hash_int16 >()
{
    return DescribeHash< int16_t >();
}


template<> Description Describe< Benchmark::hash_int32 >()
{
    return DescribeHash< int32_t >();
}


template<> Description Describe< Benchmark::hash_int64 >()
{
    return DescribeHash< int64_t >();
}


template<> Description Describe< Benchmark::hash_float >()
{
    return DescribeHash< float >();
}


template<> Description Describe< Benchmark::hash_double >()
{
    return DescribeHash< double >();
}


template<
    size_t value_len
>
static Description DescribeStringHash()
{
    std::ostringstream len_oss;
    PrettyPrintSizeof( len_oss, value_len );
    std::string len_str = len_oss.str() + "B";

    Description desc;
    desc.measures = "std::hash< std::string > over multiple values of length " + len_str + ".";

    return desc;
}


template<> Description Describe< Benchmark::hash_string_1 >()
{
    return DescribeStringHash< 1 << 0 >();
}


template<> Description Describe< Benchmark::hash_string_16 >()
{
    return DescribeStringHash< 1 << 4 >();
}


template<> Description Describe< Benchmark::hash_string_256 >()
{
    return DescribeStringHash< 1 << 8 >();
}


template<> Description Describe< Benchmark::hash_string_4k >()
{
    return DescribeStringHash< 1 << 12 >();
}


template<> Description Describe< Benchmark::hash_string_64k >()
{
    return DescribeStringHash< 1 << 16 >();
}


} // namespace bench

