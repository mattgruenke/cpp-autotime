////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements function-category benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "list.hpp"
#include "function_utils.hpp"


using namespace autotime;

namespace bench
{


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args0 >()
{
    void (*f)() = []() { Function< void * >(); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args1 >()
{
    void (*f)() = []() { Function< void *, void * >( nullptr ); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args2 >()
{
    void (*f)() = []() { Function< void *, void *, void * >( nullptr, nullptr ); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args3 >()
{
    void (*f)() = []() { Function< void *, void *, void *, void * >( nullptr, nullptr, nullptr ); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args4 >()
{
    void (*f)() = []() { Function< void *, void *, void *, void *, void * >( nullptr, nullptr, nullptr, nullptr ); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args5 >()
{
    void (*f)() = []() { Function< void *, void *, void *, void *, void *, void * >( nullptr, nullptr, nullptr, nullptr, nullptr ); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args6 >()
{
    void (*f)() = []() { Function< void *, void *, void *, void *, void *, void *, void * >( nullptr, nullptr, nullptr, nullptr, nullptr, nullptr ); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::function_args7 >()
{
    void (*f)() = []() { Function< void *, void *, void *, void *, void *, void *, void *, void * >( nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr ); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::member_function >()
{
    static Owner owner;
    void (*f)() = []() { owner.method(); };
    return { MakeTimer( f ), MakeTimer( &Overhead_void<> ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::member_function_shptr >()
{
    static std::shared_ptr< Base > p_base{ new Owner() };
    void (*f)() = []() { CallVMethod( p_base ); };
    void (*o)() = []() { CallNothing( p_base ); };
    return { MakeTimer( f ), MakeTimer( o ) };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::virtual_function >()
{
    static Base &base = Base::get();
    void (*f)() = []() { CallVMethod( base ); };
    void (*o)() = []() { CallNothing( base ); };
    return { MakeTimer( f ), MakeTimer( o ) };
}


} // namespace bench

