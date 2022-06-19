////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements functions used to customize logging behavior.
/*! @file

    See log.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/log.hpp"

#include <iostream>


namespace autotime
{


static std::ostream *ErrorStream = &std::cerr;


std::ostream &ErrorLog()
{
    return *ErrorStream;
}


std::ostream &ErrorLog( std::ostream &stream )
{
    std::ostream &previous = *ErrorStream;
    ErrorStream = &stream;
    return previous;
}


static std::ostream *DebugStream = nullptr;


std::ostream *DebugLog()
{
    return DebugStream;
}


std::ostream *DebugLog( std::ostream *stream )
{
    std::ostream *previous = DebugStream;
    DebugStream = stream;
    return previous;
}


} // namespace autotime

