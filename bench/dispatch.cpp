////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements the runtime dispatch function for creating per-benchmark timers.
/*! @file

    See dispatch.hpp, for more information.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "dispatch.hpp"

#include <stdexcept>


namespace bench
{


autotime::BenchTimers MakeTimers( Benchmark benchmark )
{
    switch (benchmark)
    {
#define CASE__MAKE_TIMERS( n ) \
    case Benchmark::n: \
        return MakeTimers< Benchmark::n >()

    CASE__MAKE_TIMERS( atomic_increment );
    CASE__MAKE_TIMERS( atomic_cmp_exchg_weak_xpctd );
    CASE__MAKE_TIMERS( atomic_cmp_exchg_weak_other );
    CASE__MAKE_TIMERS( atomic_cmp_exchg_strong_xpctd );
    CASE__MAKE_TIMERS( atomic_cmp_exchg_strong_other );
    CASE__MAKE_TIMERS( atomic_pingpong );

    CASE__MAKE_TIMERS( condvar_broadcast );
    CASE__MAKE_TIMERS( condvar_create_destroy );
    CASE__MAKE_TIMERS( condvar_signal );
    CASE__MAKE_TIMERS( condvar_signal_watched );

    CASE__MAKE_TIMERS( exception_catch_all );
    CASE__MAKE_TIMERS( exception_catch_std );
    CASE__MAKE_TIMERS( exception_noexcept );
    CASE__MAKE_TIMERS( exception_try );

    CASE__MAKE_TIMERS( function_args0 );
    CASE__MAKE_TIMERS( function_args1 );
    CASE__MAKE_TIMERS( function_args2 );
    CASE__MAKE_TIMERS( function_args3 );
    CASE__MAKE_TIMERS( function_args4 );
    CASE__MAKE_TIMERS( function_args5 );
    CASE__MAKE_TIMERS( function_args6 );
    CASE__MAKE_TIMERS( function_args7 );
    CASE__MAKE_TIMERS( member_function );
    CASE__MAKE_TIMERS( member_function_shptr );
    CASE__MAKE_TIMERS( virtual_function );

    CASE__MAKE_TIMERS( new_delete_16 );
    CASE__MAKE_TIMERS( new_delete_256 );
    CASE__MAKE_TIMERS( new_delete_4k );
    CASE__MAKE_TIMERS( new_delete_64k );
    CASE__MAKE_TIMERS( new_delete_1M );
    CASE__MAKE_TIMERS( new_delete_16M );
    CASE__MAKE_TIMERS( new_delete_256M );
    CASE__MAKE_TIMERS( new_delete_cold_16 );
    CASE__MAKE_TIMERS( new_delete_cold_4k );
    CASE__MAKE_TIMERS( new_delete_cold_1M );
    CASE__MAKE_TIMERS( new_delete_cold_256M );

    CASE__MAKE_TIMERS( istream_string4 );
    CASE__MAKE_TIMERS( istream_string64 );
    CASE__MAKE_TIMERS( istream_smallint );
    CASE__MAKE_TIMERS( istream_maxint );
    CASE__MAKE_TIMERS( istream_maxint64 );
    CASE__MAKE_TIMERS( istream_smallfloat );
    CASE__MAKE_TIMERS( istream_bigfloat );
    CASE__MAKE_TIMERS( istream_smalldouble );
    CASE__MAKE_TIMERS( istream_bigdouble );

    CASE__MAKE_TIMERS( memcpy_256 );
    CASE__MAKE_TIMERS( memcpy_4k );
    CASE__MAKE_TIMERS( memcpy_64k );
    CASE__MAKE_TIMERS( memcpy_1M );
    CASE__MAKE_TIMERS( memcpy_16M );
    CASE__MAKE_TIMERS( memcpy_256M );
    CASE__MAKE_TIMERS( strlen_256 );
    CASE__MAKE_TIMERS( strlen_4k );
    CASE__MAKE_TIMERS( strlen_64k );
    CASE__MAKE_TIMERS( strlen_1M );
    CASE__MAKE_TIMERS( strlen_16M );
    CASE__MAKE_TIMERS( strlen_256M );
    CASE__MAKE_TIMERS( memset_256 );
    CASE__MAKE_TIMERS( memset_4k );
    CASE__MAKE_TIMERS( memset_64k );
    CASE__MAKE_TIMERS( memset_1M );
    CASE__MAKE_TIMERS( memset_16M );
    CASE__MAKE_TIMERS( memset_256M );

    CASE__MAKE_TIMERS( mutex_create_destroy );
    CASE__MAKE_TIMERS( mutex_lock_release );
    CASE__MAKE_TIMERS( mutex_trylock );

    CASE__MAKE_TIMERS( ostream_string4 );
    CASE__MAKE_TIMERS( ostream_string64 );
    CASE__MAKE_TIMERS( ostream_cstr4 );
    CASE__MAKE_TIMERS( ostream_cstr64 );
    CASE__MAKE_TIMERS( ostream_smallint );
    CASE__MAKE_TIMERS( ostream_maxint );
    CASE__MAKE_TIMERS( ostream_maxint64 );
    CASE__MAKE_TIMERS( ostream_smallfloat );
    CASE__MAKE_TIMERS( ostream_bigfloat );
    CASE__MAKE_TIMERS( ostream_smalldouble );
    CASE__MAKE_TIMERS( ostream_bigdouble );
    CASE__MAKE_TIMERS( ostream_endl );

    CASE__MAKE_TIMERS( overhead_nullary );
    CASE__MAKE_TIMERS( overhead_nullary_fn );
    CASE__MAKE_TIMERS( overhead_nullary_ptr );

    CASE__MAKE_TIMERS( set_int32_copy1 );
    CASE__MAKE_TIMERS( set_int32_copy16 );
    CASE__MAKE_TIMERS( set_int32_copy256 );
    CASE__MAKE_TIMERS( set_int32_copy4k );
    CASE__MAKE_TIMERS( set_int32_copy64k );
    CASE__MAKE_TIMERS( set_int32_copy1M );
    CASE__MAKE_TIMERS( set_int32_destroy1 );
    CASE__MAKE_TIMERS( set_int32_destroy16 );
    CASE__MAKE_TIMERS( set_int32_destroy256 );
    CASE__MAKE_TIMERS( set_int32_destroy4k );
    CASE__MAKE_TIMERS( set_int32_destroy64k );
    CASE__MAKE_TIMERS( set_int32_destroy1M );
    CASE__MAKE_TIMERS( set_int32_find1 );
    CASE__MAKE_TIMERS( set_int32_find16 );
    CASE__MAKE_TIMERS( set_int32_find256 );
    CASE__MAKE_TIMERS( set_int32_find4k );
    CASE__MAKE_TIMERS( set_int32_find64k );
    CASE__MAKE_TIMERS( set_int32_find1M );
    CASE__MAKE_TIMERS( set_int32_insert16 );
    CASE__MAKE_TIMERS( set_int32_insert256 );
    CASE__MAKE_TIMERS( set_int32_insert4k );
    CASE__MAKE_TIMERS( set_int32_insert64k );
    CASE__MAKE_TIMERS( set_int32_insert1M );
    CASE__MAKE_TIMERS( set_int32_iterate16 );
    CASE__MAKE_TIMERS( set_int32_iterate256 );
    CASE__MAKE_TIMERS( set_int32_iterate4k );
    CASE__MAKE_TIMERS( set_int32_iterate64k );
    CASE__MAKE_TIMERS( set_int32_iterate1M );
    CASE__MAKE_TIMERS( set_int64_copy64k );
    CASE__MAKE_TIMERS( set_int64_destroy64k );
    CASE__MAKE_TIMERS( set_int64_find64k );
    CASE__MAKE_TIMERS( set_int64_insert4k );
    CASE__MAKE_TIMERS( set_int64_iterate64k );
    CASE__MAKE_TIMERS( set_float_copy64k );
    CASE__MAKE_TIMERS( set_float_destroy64k );
    CASE__MAKE_TIMERS( set_float_find64k );
    CASE__MAKE_TIMERS( set_float_insert4k );
    CASE__MAKE_TIMERS( set_float_iterate64k );
    CASE__MAKE_TIMERS( set_double_copy64k );
    CASE__MAKE_TIMERS( set_double_destroy64k );
    CASE__MAKE_TIMERS( set_double_find64k );
    CASE__MAKE_TIMERS( set_double_insert4k );
    CASE__MAKE_TIMERS( set_double_iterate64k );
    CASE__MAKE_TIMERS( set_string_copy64k );
    CASE__MAKE_TIMERS( set_string_destroy64k );
    CASE__MAKE_TIMERS( set_string_find64k );
    CASE__MAKE_TIMERS( set_string_insert4k );
    CASE__MAKE_TIMERS( set_string_iterate64k );

    CASE__MAKE_TIMERS( vec_int32_copy256 );
    CASE__MAKE_TIMERS( vec_int32_copy4k );
    CASE__MAKE_TIMERS( vec_int32_copy64k );
    CASE__MAKE_TIMERS( vec_int32_copy1M );
    CASE__MAKE_TIMERS( vec_int32_destroy4k );
    CASE__MAKE_TIMERS( vec_int32_destroy64k );
    CASE__MAKE_TIMERS( vec_int32_destroy1M );
    CASE__MAKE_TIMERS( vec_int32_find1 );
    CASE__MAKE_TIMERS( vec_int32_find16 );
    CASE__MAKE_TIMERS( vec_int32_find256 );
    CASE__MAKE_TIMERS( vec_int32_find4k );
    CASE__MAKE_TIMERS( vec_int32_find64k );
    CASE__MAKE_TIMERS( vec_int32_find1M );
    CASE__MAKE_TIMERS( vec_int32_insert16 );
    CASE__MAKE_TIMERS( vec_int32_insert256 );
    CASE__MAKE_TIMERS( vec_int32_insert4k );
    CASE__MAKE_TIMERS( vec_int32_insert64k );
    CASE__MAKE_TIMERS( vec_int32_insert1M );
    CASE__MAKE_TIMERS( vec_int32_iterate16 );
    CASE__MAKE_TIMERS( vec_int32_iterate256 );
    CASE__MAKE_TIMERS( vec_int32_iterate4k );
    CASE__MAKE_TIMERS( vec_int32_iterate64k );
    CASE__MAKE_TIMERS( vec_int32_iterate1M );
    CASE__MAKE_TIMERS( vec_int64_copy64k );
    CASE__MAKE_TIMERS( vec_int64_destroy64k );
    CASE__MAKE_TIMERS( vec_int64_find64k );
    CASE__MAKE_TIMERS( vec_int64_insert4k );
    CASE__MAKE_TIMERS( vec_int64_iterate64k );
    CASE__MAKE_TIMERS( vec_float_copy64k );
    CASE__MAKE_TIMERS( vec_float_destroy64k );
    CASE__MAKE_TIMERS( vec_float_find64k );
    CASE__MAKE_TIMERS( vec_float_insert4k );
    CASE__MAKE_TIMERS( vec_float_iterate64k );
    CASE__MAKE_TIMERS( vec_double_copy64k );
    CASE__MAKE_TIMERS( vec_double_destroy64k );
    CASE__MAKE_TIMERS( vec_double_find64k );
    CASE__MAKE_TIMERS( vec_double_insert4k );
    CASE__MAKE_TIMERS( vec_double_iterate64k );
    CASE__MAKE_TIMERS( vec_string_copy64k );
    CASE__MAKE_TIMERS( vec_string_destroy64k );
    CASE__MAKE_TIMERS( vec_string_find64k );
    CASE__MAKE_TIMERS( vec_string_insert4k );
    CASE__MAKE_TIMERS( vec_string_iterate64k );

    CASE__MAKE_TIMERS( string_from_smallint );
    CASE__MAKE_TIMERS( string_from_maxint );
    CASE__MAKE_TIMERS( string_from_maxint64 );
    CASE__MAKE_TIMERS( string_from_smallfloat );
    CASE__MAKE_TIMERS( string_from_bigfloat );
    CASE__MAKE_TIMERS( string_from_smalldouble );
    CASE__MAKE_TIMERS( string_from_bigdouble );

    CASE__MAKE_TIMERS( string_to_smallint );
    CASE__MAKE_TIMERS( string_to_maxint );
    CASE__MAKE_TIMERS( string_to_maxint64 );
    CASE__MAKE_TIMERS( string_to_smallfloat_os );
    CASE__MAKE_TIMERS( string_to_smallfloat_ts );
    CASE__MAKE_TIMERS( string_to_bigfloat_os );
    CASE__MAKE_TIMERS( string_to_bigfloat_ts );
    CASE__MAKE_TIMERS( string_to_smalldouble_os );
    CASE__MAKE_TIMERS( string_to_smalldouble_ts );
    CASE__MAKE_TIMERS( string_to_bigdouble_os );
    CASE__MAKE_TIMERS( string_to_bigdouble_ts );

    CASE__MAKE_TIMERS( thread_create_destroy );
    CASE__MAKE_TIMERS( thread_pingpong );

    CASE__MAKE_TIMERS( fibonacci_u64 );
    CASE__MAKE_TIMERS( mandelbrot_256 );
    CASE__MAKE_TIMERS( trigonometry );

#undef CASE__MAKE_TIMERS
    }

    throw std::runtime_error( "Unimplemented: " + ToStr( benchmark ) );
}


} // namespace bench

