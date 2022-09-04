////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Generic utility functions for use with enums.
/*! @file

    Supports enumeration of enum values and their equivalent strings.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_ENUM_UTILS_HPP
#define BENCH_ENUM_UTILS_HPP


#include <sstream>
#include <string>

#include <boost/range/irange.hpp>


namespace bench
{


template<
    typename enum_t
>
using EnumRange =
    boost::iterator_range< boost::range_detail::integer_iterator< enum_t > >;


template<
    typename enum_t
>
EnumRange< enum_t > RangeOf();


template<
    typename enum_t
>
std::ostream &List( std::ostream &ostream, const std::string &sep )
{
    const char *s = "";
    for (enum_t val: RangeOf< enum_t >())
    {
        ostream << s << val;
        s = sep.c_str();
    }

    return ostream;
}


template<
    typename enum_t
>
std::string List( const std::string &sep )
{
    std::ostringstream oss;
    List< enum_t >( oss, sep );
    return oss.str();
}


} // namespace bench


#endif  // ndef BENCH_ENUM_UTILS_HPP

