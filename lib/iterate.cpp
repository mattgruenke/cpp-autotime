////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements functions used to iterate over a callback.
/*! @file

    See iterate.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/iterate.hpp"

#include <cmath>


namespace autotime
{


steady_clock::time_point IterateUntil(
    const std::function< void() > &work, 
    const steady_clock::time_point &t_limit, 
    const steady_clock::duration &step, 
    const std::function< bool() > &predicate )
{
    // This is similar to Estimate(), but distinct in that we don't want to
    //  assume work() takes constant time.  Also, calling Estimate() will
    //  initialize the overhead model for sampling the clocks, which should be
    //  deferred until the CPU core is warm.

    int num_iters = 0;
    steady_clock::duration dur{};
    steady_clock::duration dur_prev{};
    steady_clock::time_point t = steady_clock::now();
    steady_clock::time_point t_next = t + step;

    // Get an initial estimate of f() by exponentially increasing until it exceeds 1%
    //  of the target, for 2 consecutive iterations.
    while (dur <= dur_prev || dur_prev <= step / 100)
    {
        num_iters = std::max( 2 * num_iters, 1 );
        for (int i = 0; i < num_iters; ++i) work();

        dur_prev = dur;
        dur = steady_clock::now() - t;
        t += dur;
    }

    while (t < t_limit && predicate())
    {
#if 0
        const double rate = static_cast< double >( num_iters ) / dur.count();
        num_iters = static_cast< int >( lrint( rate * (t_next - t).count() ) );
#else
        const steady_clock::duration round = dur / 2;
        num_iters = static_cast< int >( (num_iters * (t_next - t) + round) / dur );
#endif
        for (int i = 0; i < num_iters; ++i) work();

        dur = steady_clock::now() - t;
        t += dur;
        do t_next += step; while (t_next <= t);
    }

    return t;
}


} // namespace autotime

