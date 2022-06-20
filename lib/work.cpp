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
#include <cstdint>
#include <utility>


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


template< typename T > T LargestFibonacci()
{
    T prev = 0;
    T sum = 1;
    while (sum >= prev)
    {
        prev += sum;
        std::swap( prev, sum );
    }

    return prev;
}


template uint32_t LargestFibonacci< uint32_t >();

template uint64_t LargestFibonacci< uint64_t >();


} // namespace autotime

