////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements common, public types used by this library.
/*! @file

    See types.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/types.hpp"


namespace autotime
{


// struct Durations:
Durations &Durations::operator/( int denom )
{
    real   /= denom;
    thread /= denom;

    return *this;
}


Durations &Durations::operator+=( const Durations &rhs )
{
    real   += rhs.real;
    thread += rhs.thread;

    return *this;
}



// struct NormDurations:
NormDurations NormDurations::operator-( const NormDurations &rhs ) const
{
    return { this->real - rhs.real, this->thread - rhs.thread };
}



// struct DurationsForIters:
NormDurations DurationsForIters::normalize() const
{
    const NormDurations::duration round = NormDurations::duration{ num_iters } / 2;

    NormDurations result{};
    if (num_iters)
    {
        result.real   = (durs.real   + round) / num_iters;
        result.thread = (durs.thread + round) / num_iters;
    }

    return result;
}


} // namespace autotime

