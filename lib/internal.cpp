////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements internal helper functions.
/*! @file

    See internal.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "internal.hpp"

#include <cstdlib>
#include <memory>

#include <cxxabi.h>     // Possible portability issue - might need to wrap in #ifdef.


namespace autotime
{


std::string DemangledName( const std::type_info &type_info )
{
    char *buffer = nullptr;     // Specifying null forces dynamic allocation.
    size_t size = 0;
    int status = 0;
    std::unique_ptr< char, decltype( &free ) > demangled(
        abi::__cxa_demangle( type_info.name(), buffer, &size, &status ),
        &free );

    switch (status)
    {
    case 0:     // success
        return demangled.get();

    case -1:
        AUTOTIME_ERROR( "Memory allocation failure" );
        break;

    case -2:
        AUTOTIME_ERROR( "Invalid mangled name: " << type_info.name() );
        break;

    case -3:
        AUTOTIME_ERROR( "Invalid arguments to __cxa_demangle()" );
        break;

    default:
        AUTOTIME_ERROR( "Unknown status from __cxa_demangle()" );
        break;
    }

    return "<unknown>";
}


} // namespace autotime

