////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
//! Defines functions used to customize logging behavior.
/*! @file

    Any stream you configure via these functions must remain in scope either
    until the the last function in this library is called, or until
    a replacement is configured (to which the same policy applies).
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_LOG_HPP
#define AUTOTIME_LOG_HPP


#include <iosfwd>


namespace autotime
{


    //! Gets the currently-configured stream used for logging errors.
std::ostream &ErrorLog();


    //! Sets stream used for logging errors.
    /*!
        @returns previously-configured stream.

        See notes at top of log.hpp for important details regarding scope.
        Defaults to std::cerr.
    */
std::ostream &ErrorLog(
    std::ostream &stream    //! New stream.
);


    //! Gets any currently-configured stream used for debug logging.
std::ostream *DebugLog();


    //! Sets stream used for debug logging.
    /*!
        @returns previously-configured stream.

        See notes at top of log.hpp for important details regarding scope.
        Defaults to nullptr.
    */
std::ostream *DebugLog(
    std::ostream *stream    //! New stream - may be nullptr.
);


} // namespace autotime


#endif // ndef AUTOTIME_LOG_HPP

