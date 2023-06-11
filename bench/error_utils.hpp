////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Error-handling utilities.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_ERROR_UTILS_HPP
#define BENCH_ERROR_UTILS_HPP


#include <system_error>


namespace bench
{


    //! For errno values - throws std::system_error in std::generic_category().
template<
    typename message_type
>
void throw_system_error(
    int value,              //!< errno value.
    message_type &&message  //!< Error message string.
)
{
    throw std::system_error( value, std::generic_category(), message );
}


} // namespace bench


#endif  // ndef BENCH_ERROR_UTILS_HPP

