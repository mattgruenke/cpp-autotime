////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements low-level timing functions.
/*! @file

    See time.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/time.hpp"


namespace autotime
{


Durations Time( const std::function< void() > &f, int num_iter )
{
    thread_clock::time_point start_thread = thread_clock::now();
    steady_clock::time_point start_steady = steady_clock::now();

    for (int i = 0; i < num_iter; ++i) f();

    return { steady_clock::now() - start_steady, thread_clock::now() - start_thread };
}


Durations Time( void (*f)(), int num_iter )
{
    thread_clock::time_point start_thread = thread_clock::now();
    steady_clock::time_point start_steady = steady_clock::now();

    for (int i = 0; i < num_iter; ++i) f();

    return { steady_clock::now() - start_steady, thread_clock::now() - start_thread };
}


Durations detail::TimeMember(
    void(detail::ProxyType::*f)() const, const detail::ProxyType *inst, int num_iter )
{
    thread_clock::time_point start_thread = thread_clock::now();
    steady_clock::time_point start_steady = steady_clock::now();

    for (int i = 0; i < num_iter; ++i) (inst->*f)();

    return { steady_clock::now() - start_steady, thread_clock::now() - start_thread };
}


} // namespace autotime

