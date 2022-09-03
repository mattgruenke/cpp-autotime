////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Info & diagnostic tool for determining system-dependent parameters.
/*! @file

    This is both a test program for some portions of os.hpp and an informational tool for
    inspecting parameters of the underlying system.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/os.hpp"
#include "autotime/version.hpp"

#include <iostream>


int main()
{
    using msec_double = std::chrono::duration< double, std::milli >;
    double msec = std::chrono::duration_cast< msec_double >( autotime::GetTimeslice() ).count();

    std::cout << "Autotime version: " << AUTOTIME_VERSION_STRING << "\n";

    std::cout << "Timeslice is " << msec << " ms.\n";

    return 0;
}

