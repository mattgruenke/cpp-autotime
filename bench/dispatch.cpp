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

    CASE__MAKE_TIMERS( asio_dispatch );
    CASE__MAKE_TIMERS( asio_dispatch_wait );
    CASE__MAKE_TIMERS( asio_dispatch_strand );
    CASE__MAKE_TIMERS( asio_dispatch_strand_wait );
    CASE__MAKE_TIMERS( asio_dispatch_threaded );
    CASE__MAKE_TIMERS( asio_dispatch_threaded_wait );
    CASE__MAKE_TIMERS( asio_dispatch_strand_threaded_wait );
    CASE__MAKE_TIMERS( asio_post );
    CASE__MAKE_TIMERS( asio_post_wait );
    CASE__MAKE_TIMERS( asio_post_threaded );
    CASE__MAKE_TIMERS( asio_post_threaded_wait );
    CASE__MAKE_TIMERS( asio_post_tail );
    CASE__MAKE_TIMERS( asio_post_tail_strand );
    CASE__MAKE_TIMERS( asio_post_pingpong );
    CASE__MAKE_TIMERS( asio_reset );
    CASE__MAKE_TIMERS( asio_run );

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

    CASE__MAKE_TIMERS( file_append );
    CASE__MAKE_TIMERS( file_close );
    CASE__MAKE_TIMERS( file_create );
    CASE__MAKE_TIMERS( file_fdatasync_0 );
    CASE__MAKE_TIMERS( file_fdatasync_1 );
    CASE__MAKE_TIMERS( file_fdatasync_block );
    CASE__MAKE_TIMERS( file_fsync_0 );
    CASE__MAKE_TIMERS( file_fsync_1 );
    CASE__MAKE_TIMERS( file_fsync_block );
    CASE__MAKE_TIMERS( file_lseek_random );
    CASE__MAKE_TIMERS( file_open_nonexistent );
    CASE__MAKE_TIMERS( file_open_ro );
    CASE__MAKE_TIMERS( file_open_rw );
    CASE__MAKE_TIMERS( file_open_trunc );
    CASE__MAKE_TIMERS( file_read_256 );
    CASE__MAKE_TIMERS( file_read_4k );
    CASE__MAKE_TIMERS( file_read_64k );
    CASE__MAKE_TIMERS( file_read_1M );
    CASE__MAKE_TIMERS( file_read_16M );
    CASE__MAKE_TIMERS( file_read_direct_4k );
    CASE__MAKE_TIMERS( file_read_direct_64k );
    CASE__MAKE_TIMERS( file_read_direct_1M );
    CASE__MAKE_TIMERS( file_read_direct_16M );
    CASE__MAKE_TIMERS( file_stat );
    CASE__MAKE_TIMERS( file_stat_nonexistent );
    CASE__MAKE_TIMERS( file_unlink );
    CASE__MAKE_TIMERS( file_write_256 );
    CASE__MAKE_TIMERS( file_write_4k );
    CASE__MAKE_TIMERS( file_write_64k );
    CASE__MAKE_TIMERS( file_write_1M );
    CASE__MAKE_TIMERS( file_write_16M );
    CASE__MAKE_TIMERS( file_write_direct_4k );
    CASE__MAKE_TIMERS( file_write_direct_64k );
    CASE__MAKE_TIMERS( file_write_direct_1M );
    CASE__MAKE_TIMERS( file_write_direct_16M );

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
    CASE__MAKE_TIMERS( strcmp_16 );
    CASE__MAKE_TIMERS( strcmp_256 );
    CASE__MAKE_TIMERS( strcmp_4k );
    CASE__MAKE_TIMERS( strcmp_64k );
    CASE__MAKE_TIMERS( strcmp_1M );
    CASE__MAKE_TIMERS( strcmp_16M );
    CASE__MAKE_TIMERS( strcmp_256M );
    CASE__MAKE_TIMERS( strlen_256 );
    CASE__MAKE_TIMERS( strlen_4k );
    CASE__MAKE_TIMERS( strlen_64k );
    CASE__MAKE_TIMERS( strlen_1M );
    CASE__MAKE_TIMERS( strlen_16M );
    CASE__MAKE_TIMERS( strlen_256M );
    CASE__MAKE_TIMERS( strncpy_16 );
    CASE__MAKE_TIMERS( strncpy_256 );
    CASE__MAKE_TIMERS( strncpy_4k );
    CASE__MAKE_TIMERS( strncpy_64k );
    CASE__MAKE_TIMERS( strncpy_1M );
    CASE__MAKE_TIMERS( strncpy_16M );
    CASE__MAKE_TIMERS( strncpy_256M );
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

    CASE__MAKE_TIMERS( pipe_open_close );
    CASE__MAKE_TIMERS( pipe_read );
    CASE__MAKE_TIMERS( pipe_write );
    CASE__MAKE_TIMERS( pipe_write_read_256 );
    CASE__MAKE_TIMERS( pipe_write_read_1k );
    CASE__MAKE_TIMERS( pipe_write_read_4k );
    CASE__MAKE_TIMERS( pipe_write_read_16k );
    CASE__MAKE_TIMERS( pipe_write_read_64k );
    CASE__MAKE_TIMERS( pipe_pingpong_256 );
    CASE__MAKE_TIMERS( pipe_pingpong_1k );
    CASE__MAKE_TIMERS( pipe_pingpong_4k );
    CASE__MAKE_TIMERS( pipe_pingpong_16k );
    CASE__MAKE_TIMERS( pipe_pingpong_64k );
    CASE__MAKE_TIMERS( pipe_asio_read );
    CASE__MAKE_TIMERS( pipe_asio_read_async );
    CASE__MAKE_TIMERS( pipe_asio_write );
    CASE__MAKE_TIMERS( pipe_asio_write_async );
    CASE__MAKE_TIMERS( pipe_asio_write_read_256 );
    CASE__MAKE_TIMERS( pipe_asio_write_read_1k );
    CASE__MAKE_TIMERS( pipe_asio_write_read_4k );
    CASE__MAKE_TIMERS( pipe_asio_write_read_16k );
    CASE__MAKE_TIMERS( pipe_asio_write_read_64k );
    CASE__MAKE_TIMERS( pipe_asio_pingpong );
    CASE__MAKE_TIMERS( pipe_asio_pingpong_threaded );

    CASE__MAKE_TIMERS( deque_int32_copy16 );
    CASE__MAKE_TIMERS( deque_int32_copy256 );
    CASE__MAKE_TIMERS( deque_int32_copy4k );
    CASE__MAKE_TIMERS( deque_int32_copy64k );
    CASE__MAKE_TIMERS( deque_int32_copy1M );
    CASE__MAKE_TIMERS( deque_int32_destroy16 );
    CASE__MAKE_TIMERS( deque_int32_destroy256 );
    CASE__MAKE_TIMERS( deque_int32_destroy4k );
    CASE__MAKE_TIMERS( deque_int32_destroy64k );
    CASE__MAKE_TIMERS( deque_int32_destroy1M );
    CASE__MAKE_TIMERS( deque_int32_find1 );
    CASE__MAKE_TIMERS( deque_int32_find16 );
    CASE__MAKE_TIMERS( deque_int32_find256 );
    CASE__MAKE_TIMERS( deque_int32_find4k );
    CASE__MAKE_TIMERS( deque_int32_find64k );
    CASE__MAKE_TIMERS( deque_int32_find1M );
    CASE__MAKE_TIMERS( deque_int32_insert16 );
    CASE__MAKE_TIMERS( deque_int32_insert256 );
    CASE__MAKE_TIMERS( deque_int32_insert4k );
    CASE__MAKE_TIMERS( deque_int32_insert64k );
    CASE__MAKE_TIMERS( deque_int32_insert1M );
    CASE__MAKE_TIMERS( deque_int32_iterate16 );
    CASE__MAKE_TIMERS( deque_int32_iterate256 );
    CASE__MAKE_TIMERS( deque_int32_iterate4k );
    CASE__MAKE_TIMERS( deque_int32_iterate64k );
    CASE__MAKE_TIMERS( deque_int32_iterate1M );
    CASE__MAKE_TIMERS( deque_int64_copy64k );
    CASE__MAKE_TIMERS( deque_int64_destroy64k );
    CASE__MAKE_TIMERS( deque_int64_find64k );
    CASE__MAKE_TIMERS( deque_int64_insert4k );
    CASE__MAKE_TIMERS( deque_int64_iterate64k );
    CASE__MAKE_TIMERS( deque_float_copy64k );
    CASE__MAKE_TIMERS( deque_float_destroy64k );
    CASE__MAKE_TIMERS( deque_float_find64k );
    CASE__MAKE_TIMERS( deque_float_insert4k );
    CASE__MAKE_TIMERS( deque_float_iterate64k );
    CASE__MAKE_TIMERS( deque_double_copy64k );
    CASE__MAKE_TIMERS( deque_double_destroy64k );
    CASE__MAKE_TIMERS( deque_double_find64k );
    CASE__MAKE_TIMERS( deque_double_insert4k );
    CASE__MAKE_TIMERS( deque_double_iterate64k );
    CASE__MAKE_TIMERS( deque_string_copy64k );
    CASE__MAKE_TIMERS( deque_string_destroy64k );
    CASE__MAKE_TIMERS( deque_string_find64k );
    CASE__MAKE_TIMERS( deque_string_insert4k );
    CASE__MAKE_TIMERS( deque_string_iterate64k );

    CASE__MAKE_TIMERS( hash_int8 );
    CASE__MAKE_TIMERS( hash_int16 );
    CASE__MAKE_TIMERS( hash_int32 );
    CASE__MAKE_TIMERS( hash_int64 );
    CASE__MAKE_TIMERS( hash_float );
    CASE__MAKE_TIMERS( hash_double );
    CASE__MAKE_TIMERS( hash_string_1 );
    CASE__MAKE_TIMERS( hash_string_16 );
    CASE__MAKE_TIMERS( hash_string_256 );
    CASE__MAKE_TIMERS( hash_string_4k );
    CASE__MAKE_TIMERS( hash_string_64k );

    CASE__MAKE_TIMERS( hashset_int32_copy16 );
    CASE__MAKE_TIMERS( hashset_int32_copy256 );
    CASE__MAKE_TIMERS( hashset_int32_copy4k );
    CASE__MAKE_TIMERS( hashset_int32_copy64k );
    CASE__MAKE_TIMERS( hashset_int32_copy1M );
    CASE__MAKE_TIMERS( hashset_int32_destroy16 );
    CASE__MAKE_TIMERS( hashset_int32_destroy256 );
    CASE__MAKE_TIMERS( hashset_int32_destroy4k );
    CASE__MAKE_TIMERS( hashset_int32_destroy64k );
    CASE__MAKE_TIMERS( hashset_int32_destroy1M );
    CASE__MAKE_TIMERS( hashset_int32_find1 );
    CASE__MAKE_TIMERS( hashset_int32_find16 );
    CASE__MAKE_TIMERS( hashset_int32_find256 );
    CASE__MAKE_TIMERS( hashset_int32_find4k );
    CASE__MAKE_TIMERS( hashset_int32_find64k );
    CASE__MAKE_TIMERS( hashset_int32_find1M );
    CASE__MAKE_TIMERS( hashset_int32_insert16 );
    CASE__MAKE_TIMERS( hashset_int32_insert256 );
    CASE__MAKE_TIMERS( hashset_int32_insert4k );
    CASE__MAKE_TIMERS( hashset_int32_insert64k );
    CASE__MAKE_TIMERS( hashset_int32_insert1M );
    CASE__MAKE_TIMERS( hashset_int32_iterate16 );
    CASE__MAKE_TIMERS( hashset_int32_iterate256 );
    CASE__MAKE_TIMERS( hashset_int32_iterate4k );
    CASE__MAKE_TIMERS( hashset_int32_iterate64k );
    CASE__MAKE_TIMERS( hashset_int32_iterate1M );
    CASE__MAKE_TIMERS( hashset_int64_copy64k );
    CASE__MAKE_TIMERS( hashset_int64_destroy64k );
    CASE__MAKE_TIMERS( hashset_int64_find64k );
    CASE__MAKE_TIMERS( hashset_int64_insert4k );
    CASE__MAKE_TIMERS( hashset_int64_iterate64k );
    CASE__MAKE_TIMERS( hashset_float_copy64k );
    CASE__MAKE_TIMERS( hashset_float_destroy64k );
    CASE__MAKE_TIMERS( hashset_float_find64k );
    CASE__MAKE_TIMERS( hashset_float_insert4k );
    CASE__MAKE_TIMERS( hashset_float_iterate64k );
    CASE__MAKE_TIMERS( hashset_double_copy64k );
    CASE__MAKE_TIMERS( hashset_double_destroy64k );
    CASE__MAKE_TIMERS( hashset_double_find64k );
    CASE__MAKE_TIMERS( hashset_double_insert4k );
    CASE__MAKE_TIMERS( hashset_double_iterate64k );
    CASE__MAKE_TIMERS( hashset_string_copy64k );
    CASE__MAKE_TIMERS( hashset_string_destroy64k );
    CASE__MAKE_TIMERS( hashset_string_find64k );
    CASE__MAKE_TIMERS( hashset_string_insert4k );
    CASE__MAKE_TIMERS( hashset_string_iterate64k );

    CASE__MAKE_TIMERS( list_int32_copy16 );
    CASE__MAKE_TIMERS( list_int32_copy256 );
    CASE__MAKE_TIMERS( list_int32_copy4k );
    CASE__MAKE_TIMERS( list_int32_copy64k );
    CASE__MAKE_TIMERS( list_int32_copy1M );
    CASE__MAKE_TIMERS( list_int32_destroy16 );
    CASE__MAKE_TIMERS( list_int32_destroy256 );
    CASE__MAKE_TIMERS( list_int32_destroy4k );
    CASE__MAKE_TIMERS( list_int32_destroy64k );
    CASE__MAKE_TIMERS( list_int32_destroy1M );
    CASE__MAKE_TIMERS( list_int32_find1 );
    CASE__MAKE_TIMERS( list_int32_find16 );
    CASE__MAKE_TIMERS( list_int32_find256 );
    CASE__MAKE_TIMERS( list_int32_find4k );
    CASE__MAKE_TIMERS( list_int32_find64k );
    CASE__MAKE_TIMERS( list_int32_find1M );
    CASE__MAKE_TIMERS( list_int32_insert16 );
    CASE__MAKE_TIMERS( list_int32_insert256 );
    CASE__MAKE_TIMERS( list_int32_insert4k );
    CASE__MAKE_TIMERS( list_int32_insert64k );
    CASE__MAKE_TIMERS( list_int32_insert1M );
    CASE__MAKE_TIMERS( list_int32_iterate16 );
    CASE__MAKE_TIMERS( list_int32_iterate256 );
    CASE__MAKE_TIMERS( list_int32_iterate4k );
    CASE__MAKE_TIMERS( list_int32_iterate64k );
    CASE__MAKE_TIMERS( list_int32_iterate1M );
    CASE__MAKE_TIMERS( list_int64_copy64k );
    CASE__MAKE_TIMERS( list_int64_destroy64k );
    CASE__MAKE_TIMERS( list_int64_find64k );
    CASE__MAKE_TIMERS( list_int64_insert4k );
    CASE__MAKE_TIMERS( list_int64_iterate64k );
    CASE__MAKE_TIMERS( list_float_copy64k );
    CASE__MAKE_TIMERS( list_float_destroy64k );
    CASE__MAKE_TIMERS( list_float_find64k );
    CASE__MAKE_TIMERS( list_float_insert4k );
    CASE__MAKE_TIMERS( list_float_iterate64k );
    CASE__MAKE_TIMERS( list_double_copy64k );
    CASE__MAKE_TIMERS( list_double_destroy64k );
    CASE__MAKE_TIMERS( list_double_find64k );
    CASE__MAKE_TIMERS( list_double_insert4k );
    CASE__MAKE_TIMERS( list_double_iterate64k );
    CASE__MAKE_TIMERS( list_string_copy64k );
    CASE__MAKE_TIMERS( list_string_destroy64k );
    CASE__MAKE_TIMERS( list_string_find64k );
    CASE__MAKE_TIMERS( list_string_insert4k );
    CASE__MAKE_TIMERS( list_string_iterate64k );

    CASE__MAKE_TIMERS( set_int32_copy16 );
    CASE__MAKE_TIMERS( set_int32_copy256 );
    CASE__MAKE_TIMERS( set_int32_copy4k );
    CASE__MAKE_TIMERS( set_int32_copy64k );
    CASE__MAKE_TIMERS( set_int32_copy1M );
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

    CASE__MAKE_TIMERS( vec_int32_copy4k );
    CASE__MAKE_TIMERS( vec_int32_copy64k );
    CASE__MAKE_TIMERS( vec_int32_copy1M );
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

