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
    directory,
    file,
    function,
    heap,
    memory,
    mutex,
    network,
    process,
    socket,
    std_hashset,
    std_list,
    std_set,
    std_string,
    std_vector,
    syscall,
    thread,
    warmup, last = warmup
};

Category operator++( Category &c );

template<> EnumRange< Category > RangeOf< Category >();

std::istream &operator>>( std::istream &istream, Category &c );
std::ostream &operator<<( std::ostream &ostream, Category c );


enum class Benchmark
{
    // memory:
    memcpy, first = memcpy,
    memread,
    memset,

    // warmup:
    fibonacci_u64,
    mandelbrot_256,
    trigonometry, last = trigonometry
};

Benchmark operator++( Benchmark &b );

template<> EnumRange< Benchmark > RangeOf< Benchmark >();

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

std::istream &operator>>( std::istream &istream, ListMode &mode );
std::ostream &operator<<( std::ostream &ostream, ListMode mode );


    //! Prints a list of benchmarks/categories to an ostream.
std::ostream &PrintList(
    std::ostream &ostream, const std::set< Benchmark > &list, ListMode mode );


} // namespace bench


#endif  // ndef BENCH_LIST_HPP

