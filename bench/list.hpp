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
    memory,
    mutex,
    network,
    overhead,
    process,
    socket,
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

    // overhead:
    overhead_nullary,
    overhead_nullary_fn,
    overhead_nullary_ptr,

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

