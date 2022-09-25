////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements exception-category benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <exception>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "list.hpp"
#include "exception_utils.hpp"


using namespace autotime;

namespace bench
{


template<> autotime::BenchTimers MakeTimers< Benchmark::exception_noexcept >()
{
    void (*f)() = []()
        {
            no_catch( &Overhead_void<> );
        };

    void (*o)() = []()
        {
            no_catch_noexcept( &noexcept_func );
        };

    return { MakeTimer( f ), MakeTimer( o ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::exception_try >()
{
    void (*f)() = []()
        {
            catch_std( &Overhead_void<> );
        };

    void (*o)() = []()
        {
            no_catch( &Overhead_void<> );
        };

    return { MakeTimer( f ), MakeTimer( o ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::exception_catch_all >()
{
    void (*f)() = []()
        {
            void (*g)() = []() { throw int{}; };
            catch_all( g );
        };

    void (*o)() = []()
        {
            catch_all( &Overhead_void<> );
        };

    return { MakeTimer( f ), MakeTimer( o ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::exception_catch_std >()
{
    void (*f)() = []()
        {
            void (*g)() = []() { throw std::exception{}; };
            catch_std( g );
        };

    void (*o)() = []()
        {
            catch_std( &Overhead_void<> );
        };

    return { MakeTimer( f ), MakeTimer( o ) };
}


} // namespace bench

