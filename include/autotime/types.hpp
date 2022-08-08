////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Defines common, public types used by this library.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_TYPES_HPP
#define AUTOTIME_TYPES_HPP


#include <autotime/clocks.hpp>


namespace autotime
{


    //! A bundle of timing information returned by Time().
struct Durations
{
    steady_clock::duration real;    //!< Cumulative realtime execution time.
    thread_clock::duration thread;  //!< Cumulative thread execution time.
};


} // namespace autotime


#endif // ndef AUTOTIME_TYPES_HPP

