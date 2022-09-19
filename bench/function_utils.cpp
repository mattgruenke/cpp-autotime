////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Support for function benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "function_utils.hpp"


    // Disable warning of uninitialized value, since it's only the function
    //  call/return overhead we want to subtract.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"


namespace bench
{


template<
    typename return_t,
    typename... params_t
>
return_t Function( params_t... )
{
    return_t rtn;
    return rtn;
}


template void *Function< void * >();
template void *Function< void *, void * >( void * );
template void *Function< void *, void *, void * >( void *, void * );
template void *Function< void *, void *, void *, void * >( void *, void *, void * );
template void *Function< void *, void *, void *, void *, void * >( void *, void *, void *, void * );
template void *Function< void *, void *, void *, void *, void *, void * >( void *, void *, void *, void *, void * );
template void *Function< void *, void *, void *, void *, void *, void *, void * >( void *, void *, void *, void *, void *, void * );
template void *Function< void *, void *, void *, void *, void *, void *, void *, void * >( void *, void *, void *, void *, void *, void *, void * );


// struct Base:
Base::~Base()
{
}


Base &Base::get()
{
    static Owner o;
    return o;
}


void CallVMethod( Base &base )
{
    base.vmethod();
}


void CallVMethod( const std::shared_ptr< Base > &p_base )
{
    p_base->vmethod();
}


void CallNothing( Base & )
{
}


void CallNothing( const std::shared_ptr< Base > & )
{
}



// struct Owner:
void Owner::method()
{
}


void Owner::vmethod()
{
}


} // namespace bench

#pragma GCC diagnostic pop

