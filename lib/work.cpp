////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements various busy functions.
/*! @file

    See work.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/work.hpp"

#include <cmath>


namespace autotime
{


float TrigonometryFunctions( float tangent )
{
    /*
        s = o / h
        c = a / h
        t = o / a

        t = s / c
        s = t * c
        c = s / t
    */
    float angle = atanf( tangent );
    return tanf( acosf( sinf( angle ) / tanf( angle ) ) );
}


} // namespace autotime

