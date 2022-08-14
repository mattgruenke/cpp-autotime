////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements functions for estimating iteration counts; needed by AutoTime().
/*! @file

    See estimate.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/estimate.hpp"
#include "autotime/time.hpp"


namespace autotime
{


DurationsForIters Estimate( const Timer &timer, const steady_clock::duration &target )
{
    int num_iters = 0;
    Durations durs{};
    steady_clock::duration prev{};

    // Get an initial estimate of f() by exponentially increasing until it exceeds 0.1%
    //  of the target for 2 subsequent iterations.
    while (durs.real <= prev && prev <= target / 1000)
    {
        num_iters = std::max( 2 * num_iters, 1 );
        prev = durs.real;
        durs = timer( num_iters );
    }

    // Starting with the initial estimate, iteratively converge on the target.
    int num_attempts = 0;
    while (durs.real < target * 4 / 5 || durs.real > target * 2)
    {
        // To deal with possible non-convergence, limit number of attempts.
        if (++num_attempts > 5) break;

        steady_clock::duration round = durs.real / 2;
        int num_iters_next = static_cast< int >( (num_iters * target + round) / durs.real );

        // If num_iters drops too low, then it might not be possible to stay
        //  within tolerance of the target.
        if (num_iters_next <= 3) break;

        num_iters = num_iters_next;
        durs = timer( num_iters );
    }

    return { num_iters, durs };
}


} // namespace autotime

