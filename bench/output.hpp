////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Output utilities.
/*! @file

    Utility functions & types to format benchmark results.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_OUTPUT_HPP
#define BENCH_OUTPUT_HPP


#include <iosfwd>

#include "enum_utils.hpp"


namespace bench
{


enum class Format
{
    pretty,
    csv
};

Format operator++( Format &f );

template<> EnumRange< Format > RangeOf< Format >();

std::istream &operator>>( std::istream &istream, Format &f );
std::ostream &operator<<( std::ostream &ostream, Format f );


} // namespace bench


#endif  // ndef BENCH_OUTPUT_HPP

