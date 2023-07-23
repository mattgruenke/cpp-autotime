////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Supports benchmark enumeration and selection.
/*! @file

    See list.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "list.hpp"
#include "enum_impl.hpp"

#include <stdexcept>

#ifdef CASE
#   undef CASE
#endif


namespace bench
{


// enum class Category:
Category operator++( Category &c )
{
    c = Next< Category >( c );
    return c;
}


template<> EnumRange< Category > RangeOf< Category >()
{
    return boost::irange< Category >( Category::first, boost::next( Category::last ) );
}


const char *ToCStr( Category c )
{
    switch (c)
    {
#define CASE( n ) \
    case Category::n: \
        return #n

    CASE( asio );
    CASE( atomic );
    CASE( cache );
    CASE( condvar );
    CASE( exception );
    CASE( directory );
    CASE( file );
    CASE( function );
    CASE( heap );
    CASE( istream );
    CASE( memory );
    CASE( mutex );
    CASE( network );
    CASE( ostream );
    CASE( overhead );
    CASE( pipe );
    CASE( process );
    CASE( socket );
    CASE( std_deque );
    CASE( std_hashset );
    CASE( std_list );
    CASE( std_set );
    CASE( std_string );
    CASE( std_vector );
    CASE( string_from );
    CASE( string_to );
    CASE( syscall );
    CASE( thread );
    CASE( work );

#undef CASE
    }

    return nullptr;
}


std::istream &operator>>( std::istream &istream, Category &c )
{
    std::string str;
    if (istream >> str)
    {
        if (boost::optional< Category > opt = FromString< Category >( str ))
        {
            c = *opt;
            return istream;
        }
        istream.clear( std::ostream::failbit );
    }

    return istream;
}


std::ostream &operator<<( std::ostream &ostream, Category c )
{
    if (const char *c_str = ToCStr( c )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}



// enum class Benchmark:
Benchmark operator++( Benchmark &b )
{
    b = Next< Benchmark >( b );
    return b;
}


template<> EnumRange< Benchmark > RangeOf< Benchmark >()
{
    return boost::irange< Benchmark >( Benchmark::first, boost::next( Benchmark::last ) );
}


const char *ToCStr( Benchmark b )
{
    switch (b)
    {
#define CASE( n ) \
    case Benchmark::n: \
        return #n

    CASE( asio_dispatch );
    CASE( asio_dispatch_strand );
    CASE( asio_post );
    CASE( asio_post_strand );
    CASE( asio_post_pingpong );

    CASE( atomic_increment );
    CASE( atomic_cmp_exchg_weak_xpctd );
    CASE( atomic_cmp_exchg_weak_other );
    CASE( atomic_cmp_exchg_strong_xpctd );
    CASE( atomic_cmp_exchg_strong_other );
    CASE( atomic_pingpong );

    CASE( condvar_broadcast );
    CASE( condvar_create_destroy );
    CASE( condvar_signal );
    CASE( condvar_signal_watched );

    CASE( exception_catch_all );
    CASE( exception_catch_std );
    CASE( exception_noexcept );
    CASE( exception_try );

    CASE( function_args0 );
    CASE( function_args1 );
    CASE( function_args2 );
    CASE( function_args3 );
    CASE( function_args4 );
    CASE( function_args5 );
    CASE( function_args6 );
    CASE( function_args7 );
    CASE( member_function );
    CASE( member_function_shptr );
    CASE( virtual_function );

    CASE( new_delete_16 );
    CASE( new_delete_256 );
    CASE( new_delete_4k );
    CASE( new_delete_64k );
    CASE( new_delete_1M );
    CASE( new_delete_16M );
    CASE( new_delete_256M );
    CASE( new_delete_cold_16 );
    CASE( new_delete_cold_4k );
    CASE( new_delete_cold_1M );
    CASE( new_delete_cold_256M );

    CASE( istream_string4 );
    CASE( istream_string64 );
    CASE( istream_smallint );
    CASE( istream_maxint );
    CASE( istream_maxint64 );
    CASE( istream_smallfloat );
    CASE( istream_bigfloat );
    CASE( istream_smalldouble );
    CASE( istream_bigdouble );

    CASE( memcpy_256  );
    CASE( memcpy_4k   );
    CASE( memcpy_64k  );
    CASE( memcpy_1M   );
    CASE( memcpy_16M  );
    CASE( memcpy_256M );
    CASE( strlen_256  );
    CASE( strlen_4k   );
    CASE( strlen_64k  );
    CASE( strlen_1M   );
    CASE( strlen_16M  );
    CASE( strlen_256M );
    CASE( memset_256  );
    CASE( memset_4k   );
    CASE( memset_64k  );
    CASE( memset_1M   );
    CASE( memset_16M  );
    CASE( memset_256M );

    CASE( mutex_create_destroy );
    CASE( mutex_lock_release );
    CASE( mutex_trylock );

    CASE( ostream_string4 );
    CASE( ostream_string64 );
    CASE( ostream_cstr4 );
    CASE( ostream_cstr64 );
    CASE( ostream_smallint );
    CASE( ostream_maxint );
    CASE( ostream_maxint64 );
    CASE( ostream_smallfloat );
    CASE( ostream_bigfloat );
    CASE( ostream_smalldouble );
    CASE( ostream_bigdouble );
    CASE( ostream_endl );

    CASE( overhead_nullary );
    CASE( overhead_nullary_fn );
    CASE( overhead_nullary_ptr );

    CASE( pipe_read );
    CASE( pipe_write );
    CASE( pipe_write_read_256 );
    CASE( pipe_write_read_1k );
    CASE( pipe_write_read_4k );
    CASE( pipe_write_read_16k );
    CASE( pipe_write_read_64k );
    CASE( pipe_pingpong_256 );
    CASE( pipe_pingpong_1k );
    CASE( pipe_pingpong_4k );
    CASE( pipe_pingpong_16k );
    CASE( pipe_pingpong_64k );
    CASE( pipe_asio_read );
    CASE( pipe_asio_read_async );
    CASE( pipe_asio_write );
    CASE( pipe_asio_write_async );
    CASE( pipe_asio_write_read_256 );
    CASE( pipe_asio_write_read_1k );
    CASE( pipe_asio_write_read_4k );
    CASE( pipe_asio_write_read_16k );
    CASE( pipe_asio_write_read_64k );
    CASE( pipe_asio_pingpong );
    CASE( pipe_asio_pingpong_threaded );

    CASE( deque_int32_copy16 );
    CASE( deque_int32_copy256 );
    CASE( deque_int32_copy4k );
    CASE( deque_int32_copy64k );
    CASE( deque_int32_copy1M );
    CASE( deque_int32_destroy16 );
    CASE( deque_int32_destroy256 );
    CASE( deque_int32_destroy4k );
    CASE( deque_int32_destroy64k );
    CASE( deque_int32_destroy1M );
    CASE( deque_int32_find1 );
    CASE( deque_int32_find16 );
    CASE( deque_int32_find256 );
    CASE( deque_int32_find4k );
    CASE( deque_int32_find64k );
    CASE( deque_int32_find1M );
    CASE( deque_int32_insert16 );
    CASE( deque_int32_insert256 );
    CASE( deque_int32_insert4k );
    CASE( deque_int32_insert64k );
    CASE( deque_int32_insert1M );
    CASE( deque_int32_iterate16 );
    CASE( deque_int32_iterate256 );
    CASE( deque_int32_iterate4k );
    CASE( deque_int32_iterate64k );
    CASE( deque_int32_iterate1M );
    CASE( deque_int64_copy64k );
    CASE( deque_int64_destroy64k );
    CASE( deque_int64_find64k );
    CASE( deque_int64_insert4k );
    CASE( deque_int64_iterate64k );
    CASE( deque_float_copy64k );
    CASE( deque_float_destroy64k );
    CASE( deque_float_find64k );
    CASE( deque_float_insert4k );
    CASE( deque_float_iterate64k );
    CASE( deque_double_copy64k );
    CASE( deque_double_destroy64k );
    CASE( deque_double_find64k );
    CASE( deque_double_insert4k );
    CASE( deque_double_iterate64k );
    CASE( deque_string_copy64k );
    CASE( deque_string_destroy64k );
    CASE( deque_string_find64k );
    CASE( deque_string_insert4k );
    CASE( deque_string_iterate64k );

    CASE( hashset_int32_copy16 );
    CASE( hashset_int32_copy256 );
    CASE( hashset_int32_copy4k );
    CASE( hashset_int32_copy64k );
    CASE( hashset_int32_copy1M );
    CASE( hashset_int32_destroy16 );
    CASE( hashset_int32_destroy256 );
    CASE( hashset_int32_destroy4k );
    CASE( hashset_int32_destroy64k );
    CASE( hashset_int32_destroy1M );
    CASE( hashset_int32_find1 );
    CASE( hashset_int32_find16 );
    CASE( hashset_int32_find256 );
    CASE( hashset_int32_find4k );
    CASE( hashset_int32_find64k );
    CASE( hashset_int32_find1M );
    CASE( hashset_int32_insert16 );
    CASE( hashset_int32_insert256 );
    CASE( hashset_int32_insert4k );
    CASE( hashset_int32_insert64k );
    CASE( hashset_int32_insert1M );
    CASE( hashset_int32_iterate16 );
    CASE( hashset_int32_iterate256 );
    CASE( hashset_int32_iterate4k );
    CASE( hashset_int32_iterate64k );
    CASE( hashset_int32_iterate1M );
    CASE( hashset_int64_copy64k );
    CASE( hashset_int64_destroy64k );
    CASE( hashset_int64_find64k );
    CASE( hashset_int64_insert4k );
    CASE( hashset_int64_iterate64k );
    CASE( hashset_float_copy64k );
    CASE( hashset_float_destroy64k );
    CASE( hashset_float_find64k );
    CASE( hashset_float_insert4k );
    CASE( hashset_float_iterate64k );
    CASE( hashset_double_copy64k );
    CASE( hashset_double_destroy64k );
    CASE( hashset_double_find64k );
    CASE( hashset_double_insert4k );
    CASE( hashset_double_iterate64k );
    CASE( hashset_string_copy64k );
    CASE( hashset_string_destroy64k );
    CASE( hashset_string_find64k );
    CASE( hashset_string_insert4k );
    CASE( hashset_string_iterate64k );

    CASE( list_int32_copy16 );
    CASE( list_int32_copy256 );
    CASE( list_int32_copy4k );
    CASE( list_int32_copy64k );
    CASE( list_int32_copy1M );
    CASE( list_int32_destroy16 );
    CASE( list_int32_destroy256 );
    CASE( list_int32_destroy4k );
    CASE( list_int32_destroy64k );
    CASE( list_int32_destroy1M );
    CASE( list_int32_find1 );
    CASE( list_int32_find16 );
    CASE( list_int32_find256 );
    CASE( list_int32_find4k );
    CASE( list_int32_find64k );
    CASE( list_int32_find1M );
    CASE( list_int32_insert16 );
    CASE( list_int32_insert256 );
    CASE( list_int32_insert4k );
    CASE( list_int32_insert64k );
    CASE( list_int32_insert1M );
    CASE( list_int32_iterate16 );
    CASE( list_int32_iterate256 );
    CASE( list_int32_iterate4k );
    CASE( list_int32_iterate64k );
    CASE( list_int32_iterate1M );
    CASE( list_int64_copy64k );
    CASE( list_int64_destroy64k );
    CASE( list_int64_find64k );
    CASE( list_int64_insert4k );
    CASE( list_int64_iterate64k );
    CASE( list_float_copy64k );
    CASE( list_float_destroy64k );
    CASE( list_float_find64k );
    CASE( list_float_insert4k );
    CASE( list_float_iterate64k );
    CASE( list_double_copy64k );
    CASE( list_double_destroy64k );
    CASE( list_double_find64k );
    CASE( list_double_insert4k );
    CASE( list_double_iterate64k );
    CASE( list_string_copy64k );
    CASE( list_string_destroy64k );
    CASE( list_string_find64k );
    CASE( list_string_insert4k );
    CASE( list_string_iterate64k );

    CASE( set_int32_copy16 );
    CASE( set_int32_copy256 );
    CASE( set_int32_copy4k );
    CASE( set_int32_copy64k );
    CASE( set_int32_copy1M );
    CASE( set_int32_destroy16 );
    CASE( set_int32_destroy256 );
    CASE( set_int32_destroy4k );
    CASE( set_int32_destroy64k );
    CASE( set_int32_destroy1M );
    CASE( set_int32_find1 );
    CASE( set_int32_find16 );
    CASE( set_int32_find256 );
    CASE( set_int32_find4k );
    CASE( set_int32_find64k );
    CASE( set_int32_find1M );
    CASE( set_int32_insert16 );
    CASE( set_int32_insert256 );
    CASE( set_int32_insert4k );
    CASE( set_int32_insert64k );
    CASE( set_int32_insert1M );
    CASE( set_int32_iterate16 );
    CASE( set_int32_iterate256 );
    CASE( set_int32_iterate4k );
    CASE( set_int32_iterate64k );
    CASE( set_int32_iterate1M );
    CASE( set_int64_copy64k );
    CASE( set_int64_destroy64k );
    CASE( set_int64_find64k );
    CASE( set_int64_insert4k );
    CASE( set_int64_iterate64k );
    CASE( set_float_copy64k );
    CASE( set_float_destroy64k );
    CASE( set_float_find64k );
    CASE( set_float_insert4k );
    CASE( set_float_iterate64k );
    CASE( set_double_copy64k );
    CASE( set_double_destroy64k );
    CASE( set_double_find64k );
    CASE( set_double_insert4k );
    CASE( set_double_iterate64k );
    CASE( set_string_copy64k );
    CASE( set_string_destroy64k );
    CASE( set_string_find64k );
    CASE( set_string_insert4k );
    CASE( set_string_iterate64k );

    CASE( vec_int32_copy4k );
    CASE( vec_int32_copy64k );
    CASE( vec_int32_copy1M );
    CASE( vec_int32_destroy64k );
    CASE( vec_int32_destroy1M );
    CASE( vec_int32_find1 );
    CASE( vec_int32_find16 );
    CASE( vec_int32_find256 );
    CASE( vec_int32_find4k );
    CASE( vec_int32_find64k );
    CASE( vec_int32_find1M );
    CASE( vec_int32_insert16 );
    CASE( vec_int32_insert256 );
    CASE( vec_int32_insert4k );
    CASE( vec_int32_insert64k );
    CASE( vec_int32_insert1M );
    CASE( vec_int32_iterate16 );
    CASE( vec_int32_iterate256 );
    CASE( vec_int32_iterate4k );
    CASE( vec_int32_iterate64k );
    CASE( vec_int32_iterate1M );
    CASE( vec_int64_copy64k );
    CASE( vec_int64_destroy64k );
    CASE( vec_int64_find64k );
    CASE( vec_int64_insert4k );
    CASE( vec_int64_iterate64k );
    CASE( vec_float_copy64k );
    CASE( vec_float_destroy64k );
    CASE( vec_float_find64k );
    CASE( vec_float_insert4k );
    CASE( vec_float_iterate64k );
    CASE( vec_double_copy64k );
    CASE( vec_double_destroy64k );
    CASE( vec_double_find64k );
    CASE( vec_double_insert4k );
    CASE( vec_double_iterate64k );
    CASE( vec_string_copy64k );
    CASE( vec_string_destroy64k );
    CASE( vec_string_find64k );
    CASE( vec_string_insert4k );
    CASE( vec_string_iterate64k );

    CASE( string_from_smallint );
    CASE( string_from_maxint );
    CASE( string_from_maxint64 );
    CASE( string_from_smallfloat );
    CASE( string_from_bigfloat );
    CASE( string_from_smalldouble );
    CASE( string_from_bigdouble );

    CASE( string_to_smallint );
    CASE( string_to_maxint );
    CASE( string_to_maxint64 );
    CASE( string_to_smallfloat_os );
    CASE( string_to_smallfloat_ts );
    CASE( string_to_bigfloat_os );
    CASE( string_to_bigfloat_ts );
    CASE( string_to_smalldouble_os );
    CASE( string_to_smalldouble_ts );
    CASE( string_to_bigdouble_os );
    CASE( string_to_bigdouble_ts );

    CASE( thread_create_destroy );
    CASE( thread_pingpong );

    CASE( fibonacci_u64 );
    CASE( mandelbrot_256 );
    CASE( trigonometry );

#undef CASE
    }

    return nullptr;
}


std::istream &operator>>( std::istream &istream, Benchmark &b )
{
    std::string str;
    if (istream >> str)
    {
        if (boost::optional< Benchmark > opt = FromString< Benchmark >( str ))
        {
            b = *opt;
            return istream;
        }
        istream.clear( std::ostream::failbit );
    }

    return istream;
}


std::ostream &operator<<( std::ostream &ostream, Benchmark b )
{
    if (const char *c_str = ToCStr( b )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}



// CategoryBenchmarkMap():
const std::map< Category, std::set< Benchmark > > &CategoryBenchmarkMap()
{
    static const std::map< Category, std::set< Benchmark > > map =
        {
            {
                Category::asio,
                {
                    Benchmark::asio_dispatch,
                    Benchmark::asio_dispatch_strand,
                    Benchmark::asio_post,
                    Benchmark::asio_post_strand,
                    Benchmark::asio_post_pingpong,
                },
            },
            {
                Category::atomic,
                {
                    Benchmark::atomic_increment,
                    Benchmark::atomic_cmp_exchg_weak_xpctd,
                    Benchmark::atomic_cmp_exchg_weak_other,
                    Benchmark::atomic_cmp_exchg_strong_xpctd,
                    Benchmark::atomic_cmp_exchg_strong_other,
                    Benchmark::atomic_pingpong,
                },
            },
            {
                Category::condvar,
                {
                    Benchmark::condvar_broadcast,
                    Benchmark::condvar_create_destroy,
                    Benchmark::condvar_signal,
                    Benchmark::condvar_signal_watched,
                },
            },
            {
                Category::exception,
                {
                    Benchmark::exception_catch_all,
                    Benchmark::exception_catch_std,
                    Benchmark::exception_noexcept,
                    Benchmark::exception_try,
                },
            },
            {
                Category::function,
                {
                    Benchmark::function_args0,
                    Benchmark::function_args1,
                    Benchmark::function_args2,
                    Benchmark::function_args3,
                    Benchmark::function_args4,
                    Benchmark::function_args5,
                    Benchmark::function_args6,
                    Benchmark::function_args7,
                    Benchmark::member_function,
                    Benchmark::member_function_shptr,
                    Benchmark::virtual_function,
                },
            },
            {
                Category::heap,
                {
                    Benchmark::new_delete_16,
                    Benchmark::new_delete_256,
                    Benchmark::new_delete_4k,
                    Benchmark::new_delete_64k,
                    Benchmark::new_delete_1M,
                    Benchmark::new_delete_16M,
                    Benchmark::new_delete_256M,
                    Benchmark::new_delete_cold_16,
                    Benchmark::new_delete_cold_4k,
                    Benchmark::new_delete_cold_1M,
                    Benchmark::new_delete_cold_256M,
                },
            },
            {
                Category::istream,
                {
                    Benchmark::istream_string4,
                    Benchmark::istream_string64,
                    Benchmark::istream_smallint,
                    Benchmark::istream_maxint,
                    Benchmark::istream_maxint64,
                    Benchmark::istream_smallfloat,
                    Benchmark::istream_bigfloat,
                    Benchmark::istream_smalldouble,
                    Benchmark::istream_bigdouble,
                },
            },
            {
                Category::memory,
                {
                    Benchmark::memcpy_256,
                    Benchmark::memcpy_4k,
                    Benchmark::memcpy_64k,
                    Benchmark::memcpy_1M,
                    Benchmark::memcpy_16M,
                    Benchmark::memcpy_256M,
                    Benchmark::strlen_256,
                    Benchmark::strlen_4k,
                    Benchmark::strlen_64k,
                    Benchmark::strlen_1M,
                    Benchmark::strlen_16M,
                    Benchmark::strlen_256M,
                    Benchmark::memset_256,
                    Benchmark::memset_4k,
                    Benchmark::memset_64k,
                    Benchmark::memset_1M,
                    Benchmark::memset_16M,
                    Benchmark::memset_256M,
                },
            },
            {
                Category::mutex,
                {
                    Benchmark::mutex_create_destroy,
                    Benchmark::mutex_lock_release,
                    Benchmark::mutex_trylock,
                },
            },
            {
                Category::ostream,
                {
                    Benchmark::ostream_string4,
                    Benchmark::ostream_string64,
                    Benchmark::ostream_cstr4,
                    Benchmark::ostream_cstr64,
                    Benchmark::ostream_smallint,
                    Benchmark::ostream_maxint,
                    Benchmark::ostream_maxint64,
                    Benchmark::ostream_smallfloat,
                    Benchmark::ostream_bigfloat,
                    Benchmark::ostream_smalldouble,
                    Benchmark::ostream_bigdouble,
                    Benchmark::ostream_endl,
                },
            },
            {
                Category::overhead,
                {
                    Benchmark::overhead_nullary,
                    Benchmark::overhead_nullary_fn,
                    Benchmark::overhead_nullary_ptr,
                },
            },
            {
                Category::pipe,
                {
                    Benchmark::pipe_read,
                    Benchmark::pipe_write,
                    Benchmark::pipe_write_read_256,
                    Benchmark::pipe_write_read_1k,
                    Benchmark::pipe_write_read_4k,
                    Benchmark::pipe_write_read_16k,
                    Benchmark::pipe_write_read_64k,
                    Benchmark::pipe_pingpong_256,
                    Benchmark::pipe_pingpong_1k,
                    Benchmark::pipe_pingpong_4k,
                    Benchmark::pipe_pingpong_16k,
                    Benchmark::pipe_pingpong_64k,
                    Benchmark::pipe_asio_read,
                    Benchmark::pipe_asio_read_async,
                    Benchmark::pipe_asio_write,
                    Benchmark::pipe_asio_write_async,
                    Benchmark::pipe_asio_write_read_256,
                    Benchmark::pipe_asio_write_read_1k,
                    Benchmark::pipe_asio_write_read_4k,
                    Benchmark::pipe_asio_write_read_16k,
                    Benchmark::pipe_asio_write_read_64k,
                    Benchmark::pipe_asio_pingpong,
                    Benchmark::pipe_asio_pingpong_threaded,
                },
            },
            {
                Category::std_deque,
                {
                    Benchmark::deque_int32_copy16,
                    Benchmark::deque_int32_copy256,
                    Benchmark::deque_int32_copy4k,
                    Benchmark::deque_int32_copy64k,
//                    Benchmark::deque_int32_copy1M,      <- Test setup takes too long.
                    Benchmark::deque_int32_destroy16,
                    Benchmark::deque_int32_destroy256,
                    Benchmark::deque_int32_destroy4k,
                    Benchmark::deque_int32_destroy64k,
//                    Benchmark::deque_int32_destroy1M,   <- Test setup takes too long.
                    Benchmark::deque_int32_find1,
                    Benchmark::deque_int32_find16,
                    Benchmark::deque_int32_find256,
                    Benchmark::deque_int32_find4k,
                    Benchmark::deque_int32_find64k,
//                    Benchmark::deque_int32_find1M,      <- Test setup takes too long.
                    Benchmark::deque_int32_insert16,
                    Benchmark::deque_int32_insert256,
                    Benchmark::deque_int32_insert4k,
                    Benchmark::deque_int32_insert64k,
//                    Benchmark::deque_int32_insert1M,    <- Test takes too long.
                    Benchmark::deque_int32_iterate16,
                    Benchmark::deque_int32_iterate256,
                    Benchmark::deque_int32_iterate4k,
                    Benchmark::deque_int32_iterate64k,
//                    Benchmark::deque_int32_iterate1M,   <- Test setup takes too long.
                    Benchmark::deque_int64_copy64k,
                    Benchmark::deque_int64_destroy64k,
                    Benchmark::deque_int64_find64k,
                    Benchmark::deque_int64_insert4k,
                    Benchmark::deque_int64_iterate64k,
                    Benchmark::deque_float_copy64k,
                    Benchmark::deque_float_destroy64k,
                    Benchmark::deque_float_find64k,
                    Benchmark::deque_float_insert4k,
                    Benchmark::deque_float_iterate64k,
                    Benchmark::deque_double_copy64k,
                    Benchmark::deque_double_destroy64k,
                    Benchmark::deque_double_find64k,
                    Benchmark::deque_double_insert4k,
                    Benchmark::deque_double_iterate64k,
                    Benchmark::deque_string_copy64k,
                    Benchmark::deque_string_destroy64k,
                    Benchmark::deque_string_find64k,
                    Benchmark::deque_string_insert4k,
                    Benchmark::deque_string_iterate64k,
                },
            },
            {
                Category::std_hashset,
                {
                    Benchmark::hashset_int32_copy16,
                    Benchmark::hashset_int32_copy256,
                    Benchmark::hashset_int32_copy4k,
                    Benchmark::hashset_int32_copy64k,
//                    Benchmark::hashset_int32_copy1M,      <- Test setup takes too long.
                    Benchmark::hashset_int32_destroy16,
                    Benchmark::hashset_int32_destroy256,
                    Benchmark::hashset_int32_destroy4k,
                    Benchmark::hashset_int32_destroy64k,
//                    Benchmark::hashset_int32_destroy1M,   <- Test setup takes too long.
                    Benchmark::hashset_int32_find1,
                    Benchmark::hashset_int32_find16,
                    Benchmark::hashset_int32_find256,
                    Benchmark::hashset_int32_find4k,
                    Benchmark::hashset_int32_find64k,
//                    Benchmark::hashset_int32_find1M,      <- Test setup takes too long.
                    Benchmark::hashset_int32_insert16,
                    Benchmark::hashset_int32_insert256,
                    Benchmark::hashset_int32_insert4k,
                    Benchmark::hashset_int32_insert64k,
//                    Benchmark::hashset_int32_insert1M,    <- Test takes too long.
                    Benchmark::hashset_int32_iterate16,
                    Benchmark::hashset_int32_iterate256,
                    Benchmark::hashset_int32_iterate4k,
                    Benchmark::hashset_int32_iterate64k,
//                    Benchmark::hashset_int32_iterate1M,   <- Test setup takes too long.
                    Benchmark::hashset_int64_copy64k,
                    Benchmark::hashset_int64_destroy64k,
                    Benchmark::hashset_int64_find64k,
                    Benchmark::hashset_int64_insert4k,
                    Benchmark::hashset_int64_iterate64k,
                    Benchmark::hashset_float_copy64k,
                    Benchmark::hashset_float_destroy64k,
                    Benchmark::hashset_float_find64k,
                    Benchmark::hashset_float_insert4k,
                    Benchmark::hashset_float_iterate64k,
                    Benchmark::hashset_double_copy64k,
                    Benchmark::hashset_double_destroy64k,
                    Benchmark::hashset_double_find64k,
                    Benchmark::hashset_double_insert4k,
                    Benchmark::hashset_double_iterate64k,
                    Benchmark::hashset_string_copy64k,
                    Benchmark::hashset_string_destroy64k,
                    Benchmark::hashset_string_find64k,
                    Benchmark::hashset_string_insert4k,
                    Benchmark::hashset_string_iterate64k,
                },
            },
            {
                Category::std_list,
                {
                    Benchmark::list_int32_copy16,
                    Benchmark::list_int32_copy256,
                    Benchmark::list_int32_copy4k,
                    Benchmark::list_int32_copy64k,
//                    Benchmark::list_int32_copy1M,      <- Test setup takes too long.
                    Benchmark::list_int32_destroy16,
                    Benchmark::list_int32_destroy256,
                    Benchmark::list_int32_destroy4k,
                    Benchmark::list_int32_destroy64k,
//                    Benchmark::list_int32_destroy1M,   <- Test setup takes too long.
                    Benchmark::list_int32_find1,
                    Benchmark::list_int32_find16,
                    Benchmark::list_int32_find256,
                    Benchmark::list_int32_find4k,
                    Benchmark::list_int32_find64k,
//                    Benchmark::list_int32_find1M,      <- Test setup takes too long.
                    Benchmark::list_int32_insert16,
                    Benchmark::list_int32_insert256,
                    Benchmark::list_int32_insert4k,
                    Benchmark::list_int32_insert64k,
//                    Benchmark::list_int32_insert1M,    <- Test takes too long.
                    Benchmark::list_int32_iterate16,
                    Benchmark::list_int32_iterate256,
                    Benchmark::list_int32_iterate4k,
                    Benchmark::list_int32_iterate64k,
//                    Benchmark::list_int32_iterate1M,   <- Test setup takes too long.
                    Benchmark::list_int64_copy64k,
                    Benchmark::list_int64_destroy64k,
                    Benchmark::list_int64_find64k,
                    Benchmark::list_int64_insert4k,
                    Benchmark::list_int64_iterate64k,
                    Benchmark::list_float_copy64k,
                    Benchmark::list_float_destroy64k,
                    Benchmark::list_float_find64k,
                    Benchmark::list_float_insert4k,
                    Benchmark::list_float_iterate64k,
                    Benchmark::list_double_copy64k,
                    Benchmark::list_double_destroy64k,
                    Benchmark::list_double_find64k,
                    Benchmark::list_double_insert4k,
                    Benchmark::list_double_iterate64k,
                    Benchmark::list_string_copy64k,
                    Benchmark::list_string_destroy64k,
                    Benchmark::list_string_find64k,
                    Benchmark::list_string_insert4k,
                    Benchmark::list_string_iterate64k,
                },
            },
            {
                Category::std_set,
                {
                    Benchmark::set_int32_copy16,
                    Benchmark::set_int32_copy256,
                    Benchmark::set_int32_copy4k,
                    Benchmark::set_int32_copy64k,
//                    Benchmark::set_int32_copy1M,      <- Test setup takes too long.
                    Benchmark::set_int32_destroy16,
                    Benchmark::set_int32_destroy256,
                    Benchmark::set_int32_destroy4k,
                    Benchmark::set_int32_destroy64k,
//                    Benchmark::set_int32_destroy1M,   <- Test setup takes too long.
                    Benchmark::set_int32_find1,
                    Benchmark::set_int32_find16,
                    Benchmark::set_int32_find256,
                    Benchmark::set_int32_find4k,
                    Benchmark::set_int32_find64k,
//                    Benchmark::set_int32_find1M,      <- Test setup takes too long.
                    Benchmark::set_int32_insert16,
                    Benchmark::set_int32_insert256,
                    Benchmark::set_int32_insert4k,
                    Benchmark::set_int32_insert64k,
//                    Benchmark::set_int32_insert1M,    <- Test takes too long.
                    Benchmark::set_int32_iterate16,
                    Benchmark::set_int32_iterate256,
                    Benchmark::set_int32_iterate4k,
                    Benchmark::set_int32_iterate64k,
//                    Benchmark::set_int32_iterate1M,   <- Test setup takes too long.
                    Benchmark::set_int64_copy64k,
                    Benchmark::set_int64_destroy64k,
                    Benchmark::set_int64_find64k,
                    Benchmark::set_int64_insert4k,
                    Benchmark::set_int64_iterate64k,
                    Benchmark::set_float_copy64k,
                    Benchmark::set_float_destroy64k,
                    Benchmark::set_float_find64k,
                    Benchmark::set_float_insert4k,
                    Benchmark::set_float_iterate64k,
                    Benchmark::set_double_copy64k,
                    Benchmark::set_double_destroy64k,
                    Benchmark::set_double_find64k,
                    Benchmark::set_double_insert4k,
                    Benchmark::set_double_iterate64k,
                    Benchmark::set_string_copy64k,
                    Benchmark::set_string_destroy64k,
                    Benchmark::set_string_find64k,
                    Benchmark::set_string_insert4k,
                    Benchmark::set_string_iterate64k,
                },
            },
            {
                Category::std_vector,
                {
                    Benchmark::vec_int32_copy4k,
                    Benchmark::vec_int32_copy64k,
//                    Benchmark::vec_int32_copy1M,      <- Test setup takes too long.
//                    Benchmark::vec_int32_destroy64k,  <- Too unstable.
//                    Benchmark::vec_int32_destroy1M,   <- Test setup takes too long.
                    Benchmark::vec_int32_find1,
                    Benchmark::vec_int32_find16,
                    Benchmark::vec_int32_find256,
                    Benchmark::vec_int32_find4k,
                    Benchmark::vec_int32_find64k,
//                    Benchmark::vec_int32_find1M,      <- Test setup takes too long.
                    Benchmark::vec_int32_insert16,
                    Benchmark::vec_int32_insert256,
                    Benchmark::vec_int32_insert4k,
                    Benchmark::vec_int32_insert64k,
//                    Benchmark::vec_int32_insert1M,    <- Test takes too long.
                    Benchmark::vec_int32_iterate16,
                    Benchmark::vec_int32_iterate256,
                    Benchmark::vec_int32_iterate4k,
                    Benchmark::vec_int32_iterate64k,
//                    Benchmark::vec_int32_iterate1M,   <- Test setup takes too long.
                    Benchmark::vec_int64_copy64k,
//                    Benchmark::vec_int64_destroy64k,    <- Too unstable.
                    Benchmark::vec_int64_find64k,
                    Benchmark::vec_int64_insert4k,
                    Benchmark::vec_int64_iterate64k,
                    Benchmark::vec_float_copy64k,
//                    Benchmark::vec_float_destroy64k,    <- Too unstable.
                    Benchmark::vec_float_find64k,
                    Benchmark::vec_float_insert4k,
                    Benchmark::vec_float_iterate64k,
                    Benchmark::vec_double_copy64k,
//                    Benchmark::vec_double_destroy64k,   <- Too unstable.
                    Benchmark::vec_double_find64k,
                    Benchmark::vec_double_insert4k,
                    Benchmark::vec_double_iterate64k,
                    Benchmark::vec_string_copy64k,
                    Benchmark::vec_string_destroy64k,
                    Benchmark::vec_string_find64k,
                    Benchmark::vec_string_insert4k,
                    Benchmark::vec_string_iterate64k,
                },
            },
            {
                Category::string_from,
                {
                    Benchmark::string_from_smallint,
                    Benchmark::string_from_maxint,
                    Benchmark::string_from_maxint64,
                    Benchmark::string_from_smallfloat,
                    Benchmark::string_from_bigfloat,
                    Benchmark::string_from_smalldouble,
                    Benchmark::string_from_bigdouble,
                },
            },
            {
                Category::string_to,
                {
                    Benchmark::string_to_smallint,
                    Benchmark::string_to_maxint,
                    Benchmark::string_to_maxint64,
                    Benchmark::string_to_smallfloat_os,
                    Benchmark::string_to_smallfloat_ts,
                    Benchmark::string_to_bigfloat_os,
                    Benchmark::string_to_bigfloat_ts,
                    Benchmark::string_to_smalldouble_os,
                    Benchmark::string_to_smalldouble_ts,
                    Benchmark::string_to_bigdouble_os,
                    Benchmark::string_to_bigdouble_ts,
                },
            },
            {
                Category::thread,
                {
                    Benchmark::thread_create_destroy,
                    Benchmark::thread_pingpong,
                },
            },
            {
                Category::work,
                {
                    Benchmark::fibonacci_u64,
                    Benchmark::mandelbrot_256,
                    Benchmark::trigonometry,
                },
            },
        };

    return map;
}



// BenchmarkCategoryMap():
const std::map< Benchmark, Category > &BenchmarkCategoryMap()
{
    // Initializes map only on first call.
    const static auto map =
        []()
        {
            std::map< Benchmark, Category > result;
            for (auto category_benchmarks: CategoryBenchmarkMap())
            {
                for (auto benchmark: category_benchmarks.second)
                {
                    result[benchmark] = category_benchmarks.first;
                }
            }
            return result;
        }();

    return map;
}



// ParseSpecification():
std::set< Benchmark > ParseSpecification( const std::string &spec )
{
    const auto &map = CategoryBenchmarkMap();

    std::set< Benchmark > result;
    std::string::size_type ofs = 0;
    do
    {
        std::string::size_type begin = spec.find_first_not_of( ',', ofs );
        if (begin == std::string::npos) break;

        ofs = spec.find_first_of( ',', begin );
        std::string::size_type len = (ofs != std::string::npos) ? ofs - begin : std::string::npos;
        std::string name = spec.substr( begin, len );

        if (name == "all")
        {
            for (Benchmark benchmark: RangeOf< Benchmark >()) result.insert( benchmark );
        }
        else if (boost::optional< Category > cat = FromString< Category >( name ))
        {
            auto category_iter = map.find( *cat );
            if (category_iter != map.end())
            {
                const auto &benchmarks = category_iter->second;
                result.insert( benchmarks.begin(), benchmarks.end() );
            }
        }
        else if (boost::optional< Benchmark > bench = FromString< Benchmark >( name ))
        {
            result.insert( *bench );
        }
        else throw std::runtime_error( "Invalid name in spec: " + name );
    }
    while (ofs != std::string::npos);

    return result;
}



// enum class ListMode:
ListMode operator++( ListMode &mode )
{
    mode = Next< ListMode >( mode );
    return mode;
}


template<> EnumRange< ListMode > RangeOf< ListMode >()
{
    return boost::irange< ListMode >( ListMode::first, boost::next( ListMode::last ) );
}


const char *ToCStr( ListMode mode )
{
    switch (mode)
    {
    case ListMode::benchmarks:
        return "benchmarks";

    case ListMode::categories:
        return "categories";

    case ListMode::joint:
        return "joint";
    }

    return nullptr;
}


std::istream &operator>>( std::istream &istream, ListMode &mode )
{
    std::string str;
    if (istream >> str)
    {
        if (boost::optional< ListMode > opt = FromString< ListMode >( str ))
        {
            mode = *opt;
            return istream;
        }
        istream.clear( std::ostream::failbit );
    }

    return istream;
}


std::ostream &operator<<( std::ostream &ostream, ListMode mode )
{
    if (const char *c_str = ToCStr( mode )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}



// Print():
std::ostream &PrintList(
    std::ostream &ostream, const std::set< Benchmark > &list, ListMode mode )
{
    switch (mode)
    {
    case ListMode::benchmarks:
        for (auto benchmark: list) ostream << benchmark << "\n";
        break;

    case ListMode::categories:
        {
            const auto &category_of = BenchmarkCategoryMap();
            std::set< Category > categories;
            for (auto benchmark: list)
            {
                auto iter = category_of.find( benchmark );
                if (iter != category_of.end()) categories.insert( iter->second );
            }
            for (auto category: categories) ostream << category << "\n";
        }
        break;

    case ListMode::joint:
        {
            const auto &category_of = BenchmarkCategoryMap();
            std::map< Category, std::set< Benchmark > > by_category;
            for (auto benchmark: list)
            {
                auto iter = category_of.find( benchmark );
                if (iter != category_of.end()) by_category[iter->second].insert( benchmark );
            }

            for (auto category_benchmarks: by_category)
            {
                ostream << category_benchmarks.first << ":\n";
                for (auto benchmark: category_benchmarks.second)
                {
                    ostream << "  " << benchmark << "\n";
                }
                ostream << "\n";
            }
        }
        break;
    }

    return ostream;
}


} // namespace bench

