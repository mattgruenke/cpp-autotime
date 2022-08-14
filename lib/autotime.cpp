////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements top-level AutoTime() API functions.
/*! @file

    See autotime.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/autotime.hpp"
#include "autotime/estimate.hpp"
#include "autotime/os.hpp"


namespace autotime
{


DurationsForIters AutoTime( const Timer &timer )
{
    // Just a trivial implementation, for now.
    //  This will improve, once support for analyzers is added.
    return Estimate( timer, GetTimeslice() );
}


} // namespace autotime

