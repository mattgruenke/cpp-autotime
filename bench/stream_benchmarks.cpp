////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements ostream-category benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "dispatch.hpp"

#include <cstdlib>
#include <limits>
#include <sstream>
#include <string>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"


using namespace autotime;

namespace bench
{


static std::ostringstream Oss;
static std::string Str;
static int32_t Int32 = 0;
static int64_t Int64 = 0;
static float Float = 0;
static float Double = 0;


static void ResetOSS()
{
    Oss.seekp( 0 );
}


static std::ostringstream MakeOSS( size_t buf_size )
{
    std::string buf;
    buf.resize( buf_size );
    return std::ostringstream( buf );
}


static void WriteStr()
{
    ResetOSS();
    Oss << Str;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_string4 >()
{
    Str = "1234";
    Oss = MakeOSS( Str.size() + 1 );

    return { MakeTimer( &WriteStr ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_string64 >()
{
    Str.clear();
    for (int i = 0; i < 64; ++i) Str.push_back( 'a' + (i % 26) );
    Oss = MakeOSS( Str.size() + 1 );

    return { MakeTimer( &WriteStr ), MakeTimer( &ResetOSS ) };
}


static void WriteCStr()
{
    ResetOSS();
    Oss << Str.c_str();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_cstr4 >()
{
    Str = "1234";
    Oss = MakeOSS( Str.size() + 1 );

    return { MakeTimer( &WriteCStr ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_cstr64 >()
{
    Str.clear();
    for (int i = 0; i < 64; ++i) Str.push_back( 'a' + (i % 26) );
    Oss = MakeOSS( Str.size() + 1 );

    return { MakeTimer( &WriteCStr ), MakeTimer( &ResetOSS ) };
}


static int32_t MakeRandomInt( int scale )
{
    return static_cast< int32_t >( random() * scale / RAND_MAX );
}


static void WriteInt32()
{   
    ResetOSS();
    Oss << Int32;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_smallint >()
{
    Int32 = MakeRandomInt( 10 );
    Oss = MakeOSS( std::to_string( Int32 ).size() + 1 );

    return { MakeTimer( &WriteInt32 ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_maxint >()
{
    Int32 = std::numeric_limits< int32_t >::max() - MakeRandomInt( 10 );
    Oss = MakeOSS( std::to_string( Int32 ).size() + 1 );

    return { MakeTimer( &WriteInt32 ), MakeTimer( &ResetOSS ) };
}


static void WriteInt64()
{   
    ResetOSS();
    Oss << Int64;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_maxint64 >()
{
    Int64 = std::numeric_limits< int64_t >::max() - MakeRandomInt( 10 );
    Oss = MakeOSS( std::to_string( Int64 ).size() + 1 );

    return { MakeTimer( &WriteInt64 ), MakeTimer( &ResetOSS ) };
}


static void WriteFloat()
{   
    ResetOSS();
    Oss << Float;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_smallfloat >()
{
    Float = MakeRandomInt( 10 );
    Oss = std::ostringstream();
    Oss << Float;
    Oss = MakeOSS( Oss.str().size() + 1 );

    return { MakeTimer( &WriteFloat ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_bigfloat >()
{
    Float = std::numeric_limits< float >::max() - MakeRandomInt( 10 );
    Oss = std::ostringstream();
    Oss << Float;
    Oss = MakeOSS( Oss.str().size() + 1 );

    return { MakeTimer( &WriteFloat ), MakeTimer( &ResetOSS ) };
}


static void WriteDouble()
{   
    ResetOSS();
    Oss << Double;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_smalldouble >()
{
    Double = MakeRandomInt( 10 );
    Oss = std::ostringstream();
    Oss << Double;
    Oss = MakeOSS( Oss.str().size() + 1 );

    return { MakeTimer( &WriteDouble ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_bigdouble >()
{
    Double = std::numeric_limits< double >::max() - MakeRandomInt( 10 );
    Oss = std::ostringstream();
    Oss << Double;
    Oss = MakeOSS( Oss.str().size() + 1 );

    return { MakeTimer( &WriteDouble ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_endl >()
{
    void (*f)() = []()
        {
            ResetOSS();
            Oss << std::endl;
        };

    Oss = std::ostringstream();
    Oss << std::endl;
    Oss = MakeOSS( Oss.str().size() + 1 );

    return { MakeTimer( f ), MakeTimer( &ResetOSS ) };
}


} // namespace bench

