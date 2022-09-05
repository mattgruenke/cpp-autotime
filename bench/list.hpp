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
    thread, last = thread
};

Category operator++( Category &c );

template<> EnumRange< Category > RangeOf< Category >();

std::istream &operator>>( std::istream &istream, Category &c );
std::ostream &operator<<( std::ostream &ostream, Category c );


enum class Benchmark
{
    memcpy, first = memcpy,
    memread,
    memset, last = memset
};

Benchmark operator++( Benchmark &b );

template<> EnumRange< Benchmark > RangeOf< Benchmark >();

std::istream &operator>>( std::istream &istream, Benchmark &b );
std::ostream &operator<<( std::ostream &ostream, Benchmark b );


} // namespace bench


#endif  // ndef BENCH_LIST_HPP

