////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Enum implementation helpers.
/*! @file

    Supports iteration of enum values and parsing from strings.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_ENUM_IMPL_HPP
#define BENCH_ENUM_IMPL_HPP


#include <boost/optional.hpp>


namespace bench
{


template<
    typename enum_t
>
enum_t Next( enum_t val )
{
    int next = static_cast< int >( val ) + 1;
    return static_cast< enum_t >( next );
}


template<
    typename enum_t
>
boost::optional< enum_t > FromString( const std::string &str )
{
    for (enum_t val: RangeOf< enum_t >())
    {
        if (str == ToString( val )) return val;
    }

    return {};
}


} // namespace bench


#endif  // ndef BENCH_ENUM_IMPL_HPP

