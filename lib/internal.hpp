////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Internal definitions and helpers.
/*! @file

    All macros defined herein are written for use within the autotime namespace.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_INTERNAL_HPP
#define AUTOTIME_INTERNAL_HPP


#include "autotime/log.hpp"

#include <cstring>
#include <ostream>
#include <string>
#include <typeinfo>


#define AUTOTIME_DEBUG( expr ) \
    if (std::ostream *dlog = DebugLog()) (*dlog) << __FUNCTION__ << ": " << expr << "\n"


#define AUTOTIME_ERROR( expr ) \
    ErrorLog() << "Error: " << expr << "\n"


#define AUTOTIME_ERRNO( expr ) \
    ErrorLog() << "Error: " << expr << " (" << strerror( errno ) << ")\n"


#define AUTOTIME_TYPENAME( type_expr ) \
    DemangledName( typeid( type_expr ) )


namespace autotime
{


    //! Internal helper function for computing demangled type name.
std::string DemangledName(
    const std::type_info &type_info     //!< From which to get demanlged name.
);


} // namespace autotime


#endif // ndef AUTOTIME_INTERNAL_HPP

