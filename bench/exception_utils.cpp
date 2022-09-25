////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Support for exception benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "exception_utils.hpp"

#include <exception>

    // Disable warning of noexcept changing function pointer type signature.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnoexcept-type"


namespace bench
{


void noexcept_func() noexcept
{
}


bool catch_std( void (*f)() )
{
    try
    {
        f();
    }
    catch (const std::exception &)
    {
        return false;
    }

    return true;
}


bool catch_all( void (*f)() )
{
    try
    {
        f();
    }
    catch (...)
    {
        return false;
    }

    return true;
}


bool no_catch( void (*f)() )
{
    f();
    return true;
}


bool no_catch_noexcept( void (*f)() noexcept ) noexcept
{
    f();
    return true;
}


} // namespace bench

#pragma GCC diagnostic pop

