////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Formatting-related utilities.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_FORMAT_UTILS_HPP
#define BENCH_FORMAT_UTILS_HPP

#include <iosfwd>


namespace bench
{


    //! Pretty prints a size with the closest power-of-2 suffix.
std::ostream &PrettyPrintSizeof( std::ostream &ostream, size_t size );


} // namespace bench


#endif  // ndef BENCH_FORMAT_UTILS_HPP

