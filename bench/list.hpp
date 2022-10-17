////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Enumerations of benchmarks and categories.
/*! @file

    Supports benchmark enumeration and selection.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_LIST_HPP
#define BENCH_LIST_HPP


#include <iosfwd>
#include <map>
#include <set>

#include "enum_utils.hpp"


namespace bench
{


enum class Category
{
    atomic, first = atomic,
    cache,
    condvar,
    directory,
    exception,
    file,
    function,
    heap,
    istream,
    memory,
    mutex,
    network,
    ostream,
    overhead,
    process,
    socket,
    string_from,
    string_to,
    std_hashset,
    std_list,
    std_set,
    std_string,
    std_vector,
    syscall,
    thread,
    work, last = work
};

Category operator++( Category &c );

template<> EnumRange< Category > RangeOf< Category >();

const char *ToCStr( Category c );

std::istream &operator>>( std::istream &istream, Category &c );
std::ostream &operator<<( std::ostream &ostream, Category c );


enum class Benchmark
{
    // atomic:
    atomic_increment, first = atomic_increment,
    atomic_cmp_exchg_weak_xpctd,
    atomic_cmp_exchg_weak_other,
    atomic_cmp_exchg_strong_xpctd,
    atomic_cmp_exchg_strong_other,
    atomic_pingpong,

    // condvar:
    condvar_broadcast,
    condvar_create_destroy,
    condvar_signal,
    condvar_signal_watched,

    // exception:
    exception_catch_all,
    exception_catch_std,
    exception_noexcept,
    exception_try,

    // function:
    function_args0,
    function_args1,
    function_args2,
    function_args3,
    function_args4,
    function_args5,
    function_args6,
    function_args7,
    member_function,
    member_function_shptr,
    virtual_function,

    // heap:
    new_delete_16,
    new_delete_256,
    new_delete_4k,
    new_delete_64k,
    new_delete_1M,
    new_delete_16M,
    new_delete_256M,
    new_delete_cold_16,
    new_delete_cold_4k,
    new_delete_cold_1M,
    new_delete_cold_256M,

    // istream:
    istream_string4,
    istream_string64,
    istream_smallint,
    istream_maxint,
    istream_maxint64,
    istream_smallfloat,
    istream_bigfloat,
    istream_smalldouble,
    istream_bigdouble,

    // memory:
    memcpy_256,
    memcpy_4k,
    memcpy_64k,
    memcpy_1M,
    memcpy_16M,
    memcpy_256M,
    strlen_256,
    strlen_4k,
    strlen_64k,
    strlen_1M,
    strlen_16M,
    strlen_256M,
    memset_256,
    memset_4k,
    memset_64k,
    memset_1M,
    memset_16M,
    memset_256M,

    // mutex:
    mutex_create_destroy,
    mutex_lock_release,
    mutex_trylock,

    // ostream:
    ostream_string4,
    ostream_string64,
    ostream_cstr4,
    ostream_cstr64,
    ostream_smallint,
    ostream_maxint,
    ostream_maxint64,
    ostream_smallfloat,
    ostream_bigfloat,
    ostream_smalldouble,
    ostream_bigdouble,
    ostream_endl,

    // overhead:
    overhead_nullary,
    overhead_nullary_fn,
    overhead_nullary_ptr,

    // std_set
    set_int32_copy16,
    set_int32_copy256,
    set_int32_copy4k,
    set_int32_copy64k,
    set_int32_copy1M,
    set_int32_destroy16,
    set_int32_destroy256,
    set_int32_destroy4k,
    set_int32_destroy64k,
    set_int32_destroy1M,
    set_int32_find1,
    set_int32_find16,
    set_int32_find256,
    set_int32_find4k,
    set_int32_find64k,
    set_int32_find1M,
    set_int32_insert16,
    set_int32_insert256,
    set_int32_insert4k,
    set_int32_insert64k,
    set_int32_insert1M,
    set_int32_iterate16,
    set_int32_iterate256,
    set_int32_iterate4k,
    set_int32_iterate64k,
    set_int32_iterate1M,
    set_int64_copy64k,
    set_int64_destroy64k,
    set_int64_find64k,
    set_int64_insert4k,
    set_int64_iterate64k,
    set_float_copy64k,
    set_float_destroy64k,
    set_float_find64k,
    set_float_insert4k,
    set_float_iterate64k,
    set_double_copy64k,
    set_double_destroy64k,
    set_double_find64k,
    set_double_insert4k,
    set_double_iterate64k,
    set_string_copy64k,
    set_string_destroy64k,
    set_string_find64k,
    set_string_insert4k,
    set_string_iterate64k,

    // std_vector
    vec_int32_copy4k,
    vec_int32_copy64k,
    vec_int32_copy1M,
    vec_int32_destroy64k,
    vec_int32_destroy1M,
    vec_int32_find1,
    vec_int32_find16,
    vec_int32_find256,
    vec_int32_find4k,
    vec_int32_find64k,
    vec_int32_find1M,
    vec_int32_insert16,
    vec_int32_insert256,
    vec_int32_insert4k,
    vec_int32_insert64k,
    vec_int32_insert1M,
    vec_int32_iterate16,
    vec_int32_iterate256,
    vec_int32_iterate4k,
    vec_int32_iterate64k,
    vec_int32_iterate1M,
    vec_int64_copy64k,
    vec_int64_destroy64k,
    vec_int64_find64k,
    vec_int64_insert4k,
    vec_int64_iterate64k,
    vec_float_copy64k,
    vec_float_destroy64k,
    vec_float_find64k,
    vec_float_insert4k,
    vec_float_iterate64k,
    vec_double_copy64k,
    vec_double_destroy64k,
    vec_double_find64k,
    vec_double_insert4k,
    vec_double_iterate64k,
    vec_string_copy64k,
    vec_string_destroy64k,
    vec_string_find64k,
    vec_string_insert4k,
    vec_string_iterate64k,

    // string_from:
    string_from_smallint,
    string_from_maxint,
    string_from_maxint64,
    string_from_smallfloat,
    string_from_bigfloat,
    string_from_smalldouble,
    string_from_bigdouble,

    // string_to:  (Note: _os versions share input formatting w/ istream tests)
    string_to_smallint,
    string_to_maxint,
    string_to_maxint64,
    string_to_smallfloat_os,
    string_to_smallfloat_ts,
    string_to_bigfloat_os,
    string_to_bigfloat_ts,
    string_to_smalldouble_os,
    string_to_smalldouble_ts,
    string_to_bigdouble_os,
    string_to_bigdouble_ts,

    // thread:
    thread_create_destroy,
    thread_pingpong,

    // work:
    fibonacci_u64,
    mandelbrot_256,
    trigonometry, last = trigonometry
};

Benchmark operator++( Benchmark &b );

template<> EnumRange< Benchmark > RangeOf< Benchmark >();

const char *ToCStr( Benchmark b );

std::istream &operator>>( std::istream &istream, Benchmark &b );
std::ostream &operator<<( std::ostream &ostream, Benchmark b );


    //! Returns a categorized list of all benchmarks.
const std::map< Category, std::set< Benchmark > > &CategoryBenchmarkMap();


    //! Returns an index of each benchmark's category.
const std::map< Benchmark, Category > &BenchmarkCategoryMap();


    //! Parses a specification string into a set of benchmarks to run.
std::set< Benchmark > ParseSpecification( const std::string &spec );


enum class ListMode
{
    benchmarks, first = benchmarks,
    categories,
    joint,      last = joint
};

ListMode operator++( ListMode &mode );

template<> EnumRange< ListMode > RangeOf< ListMode >();

const char *ToCStr( ListMode mode );

std::istream &operator>>( std::istream &istream, ListMode &mode );
std::ostream &operator<<( std::ostream &ostream, ListMode mode );


    //! Prints a list of benchmarks/categories to an ostream.
std::ostream &PrintList(
    std::ostream &ostream, const std::set< Benchmark > &list, ListMode mode );


} // namespace bench


#endif  // ndef BENCH_LIST_HPP

