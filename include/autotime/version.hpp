////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Provides library version information.
/*! @file

    This library uses semantic versioning.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_VERSION_HPP
#define AUTOTIME_VERSION_HPP


    //! Incremented for incompatible API changes.
#define AUTOTIME_VERSION_MAJOR 0

    //! Incremented for API additions.
#define AUTOTIME_VERSION_MINOR 1

    //! Incremented for fixes and enhancements not affecting the API.
#define AUTOTIME_VERSION_PATCH 0

    //! Full version number as a string literal.
#define AUTOTIME_VERSION_STRING \
    AUTOTIME_VERSION_STRINGIFY( \
        AUTOTIME_VERSION_MAJOR.AUTOTIME_VERSION_MINOR.AUTOTIME_VERSION_PATCH )

    //! Primarily for use by AUTOTIME_VERSION_STRING.
#define AUTOTIME_VERSION_STRINGIFY( expr )  AUTOTIME_VERSION_STRINGIFY_( expr )

    //! Primarily for use by AUTOTIME_VERSION_STRINGIFY().
#define AUTOTIME_VERSION_STRINGIFY_( expr )  # expr


#endif // ndef AUTOTIME_VERSION_HPP

