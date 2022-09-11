////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Runtime & compile-time dispatch functions for creating per-benchmark timers.
/*! @file

    The runtime dispatch is for use by main(), while the compile-time dispatch
    template is used as the interface between the runtime dispatch function and
    the individual benchmarks.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_DISPATCH_HPP
#define BENCH_DISPATCH_HPP


#include <autotime/types.hpp>

#include "list.hpp"


namespace bench
{


    //! Runtime dispatch function.
autotime::BenchTimers MakeTimers( Benchmark b );


    //! Compile-time dispatch function.
template<
    Benchmark b
>
autotime::BenchTimers MakeTimers();


} // namespace bench


#endif // ndef BENCH_DISPATCH_HPP

