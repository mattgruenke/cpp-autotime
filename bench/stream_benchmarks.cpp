////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements istream-, ostream-, and from_string- category benchmarks.
/*! @file

    This file originally was to contain only iostreams benchmarks, however
    the amount of similarity between those and the string conversion
    benchmarks meant there are a lot of useful static functions and variables
    that could be shared.  In addition, it's helpful if string & stream
    conversion benchmarks are measuring conversions of the same values.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "dispatch.hpp"

#include <cmath>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <string>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"


using namespace autotime;

namespace bench
{


static std::istringstream Iss;
static std::ostringstream Oss;
static std::string Str;
static std::string StrSrc;
static int32_t Int32 = 0;
static int64_t Int64 = 0;
static float Float = 0;
static float Double = 0;


static int32_t MakeRandomInt( int scale )
{
    return static_cast< int32_t >( random() * scale / RAND_MAX );
}


static void CopyStr()
{
    Str = StrSrc;
}



// Category::string_from
static void StrFromInt32()
{
    Str = std::to_string( Int32 );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::string_from_smallint >()
{
    Int32 = MakeRandomInt( 9 ) + 1;
    StrSrc = std::to_string( Int32 );

    return { MakeTimer( &StrFromInt32 ), MakeTimer( &CopyStr ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::string_from_maxint >()
{
    Int32 = std::numeric_limits< int32_t >::max() - MakeRandomInt( 10 );
    StrSrc = std::to_string( Int32 );

    return { MakeTimer( &StrFromInt32 ), MakeTimer( &CopyStr ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::string_from_maxint64 >()
{
    Int64 = std::numeric_limits< int64_t >::max() - MakeRandomInt( 10 );
    StrSrc = std::to_string( Int64 );
    void (*f)() = []()
        {
            Str = std::to_string( Int64 );
        };

    return { MakeTimer( f ), MakeTimer( &CopyStr ) };
}


static void StrFromFloat()
{
    Str = std::to_string( Float );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::string_from_smallfloat >()
{
    Float = MakeRandomInt( 9 ) + 1;
    StrSrc = std::to_string( Float );

    return { MakeTimer( &StrFromFloat ), MakeTimer( &CopyStr ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::string_from_bigfloat >()
{
    Float = exp10f( Oss.precision() ) - (MakeRandomInt( 9 ) + 1);
    StrSrc = std::to_string( Float );

    return { MakeTimer( &StrFromFloat ), MakeTimer( &CopyStr ) };
}


static void StrFromDouble()
{
    Str = std::to_string( Double );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::string_from_smalldouble >()
{
    Double = MakeRandomInt( 9 ) + 1;
    StrSrc = std::to_string( Double );

    return { MakeTimer( &StrFromDouble ), MakeTimer( &CopyStr ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::string_from_bigdouble >()
{
    Double = (1.0 - exp10( -Oss.precision() )) * 1e+38 - MakeRandomInt( 10 );
    StrSrc = std::to_string( Double );

    return { MakeTimer( &StrFromDouble ), MakeTimer( &CopyStr ) };
}



// Category::istream:
static void ResetISS()
{
    Iss.clear();
    Iss.seekg( 0 );
}


static void ReadStr()
{
    ResetISS();
    Iss >> Str;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_string4 >()
{
    Iss.str( "1234" );

    return { MakeTimer( &ReadStr ), MakeTimer( &ResetISS ) };
}


static std::string MakeString( int len )
{
    std::string result;
    for (int i = 0; i < len; ++i) result.push_back( 'a' + (i % 26) );
    return result;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_string64 >()
{
    Iss.str( MakeString( 64 ) );

    return { MakeTimer( &ReadStr ), MakeTimer( &ResetISS ) };
}


static void ReadInt32()
{
    ResetISS();
    Iss >> Int32;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_smallint >()
{
    Iss.str( std::to_string( MakeRandomInt( 9 ) + 1 ) );

    return { MakeTimer( &ReadInt32 ), MakeTimer( &ResetISS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_maxint >()
{
    Iss.str( std::to_string( std::numeric_limits< int32_t >::max() - MakeRandomInt( 10 ) ) );

    return { MakeTimer( &ReadInt32 ), MakeTimer( &ResetISS ) };
}


static void ReadInt64()
{
    ResetISS();
    Iss >> Int64;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_maxint64 >()
{
    Iss.str( std::to_string( std::numeric_limits< int64_t >::max() - MakeRandomInt( 10 ) ) );

    return { MakeTimer( &ReadInt64 ), MakeTimer( &ResetISS ) };
}


static void ReadFloat()
{
    ResetISS();
    Iss >> Float;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_smallfloat >()
{
    Iss.str( std::to_string( MakeRandomInt( 10 ) ) );

    return { MakeTimer( &ReadFloat ), MakeTimer( &ResetISS ) };
}


template< typename T > static std::string ToString( T val )
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_bigfloat >()
{
    Iss.str( ToString( exp10f( Oss.precision() ) - (MakeRandomInt( 9 ) + 1) ) );

    return { MakeTimer( &ReadFloat ), MakeTimer( &ResetISS ) };
}


static void ReadDouble()
{
    ResetISS();
    Iss >> Double;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_smalldouble >()
{
    Iss.str( std::to_string( MakeRandomInt( 10 ) ) );

    return { MakeTimer( &ReadDouble ), MakeTimer( &ResetISS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::istream_bigdouble >()
{
    Iss.str( ToString( (1.0 - exp10( -Oss.precision() )) * 1e+38 - MakeRandomInt( 10 ) ) );

    return { MakeTimer( &ReadDouble ), MakeTimer( &ResetISS ) };
}



// Category::ostream:
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
    Str = MakeString( 64 );
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
    Str = MakeString( 64 );
    Oss = MakeOSS( Str.size() + 1 );

    return { MakeTimer( &WriteCStr ), MakeTimer( &ResetOSS ) };
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
    Oss = MakeOSS( ToString( Float ).size() + 1 );

    return { MakeTimer( &WriteFloat ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_bigfloat >()
{
    Float = exp10( Oss.precision() ) - (MakeRandomInt( 9 ) + 1);
    Oss = MakeOSS( ToString( Float ).size() + 1 );

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
    Oss = MakeOSS( ToString( Double ).size() + 1 );

    return { MakeTimer( &WriteDouble ), MakeTimer( &ResetOSS ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::ostream_bigdouble >()
{
    Double = (1.0 - exp10( -Oss.precision() )) * 1e+38 - MakeRandomInt( 10 );
    Oss = MakeOSS( ToString( Double ).size() + 1 );

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

