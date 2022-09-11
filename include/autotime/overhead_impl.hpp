////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements generic mechanisms for approximating function call overhead.
/*! @file

    See overhead.hpp, for additional information.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_OVERHEAD_IMPL_HPP
#define AUTOTIME_OVERHEAD_IMPL_HPP


#include <autotime/overhead.hpp>

    // Disable warning of uninitialized value, since it's only the function
    //  call/return overhead we want to subtract.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"


namespace autotime
{


template<
    typename return_t,
    typename... params_t
>
return_t Overhead(
    params_t...
)
{
    return_t rtn;
    return rtn;
}


template<
    typename... params_t
>
void Overhead_void(
    params_t...
)
{
}


template<
    typename return_t
>
struct OverheadFactory
{
    template<
        typename... params_t
    >
    static std::function< return_t( params_t... ) > makeFn()
    {
        return []( params_t... ) -> return_t
            {
                return_t rtn;
                return rtn;
            };
    }

    template<
        typename... params_t
    >
    static return_t(*makePtr( params_t... ))()
    {
        return []( params_t... ) -> return_t
            {
                return_t rtn;
                return rtn;
            };
    }
};


template<> struct OverheadFactory< void >
{
    template<
        typename... params_t
    >
    static std::function< void( params_t... ) > makeFn()
    {
        return []( params_t... ) -> void {};
    }

    template<
        typename... params_t
    >
    static void(*makePtr( params_t... ))()
    {
        return []( params_t... ) -> void {};
    }
};


template<
    typename return_t,
    typename... params_t
>
std::function< return_t( params_t... ) > MakeOverheadFn()
{
    return OverheadFactory< return_t >::template makeFn< params_t... >();
}


template<
    typename return_t,
    typename... params_t
>
return_t(*MakeOverheadPtr( params_t... ))()
{
    return OverheadFactory< return_t >::template makePtr< params_t... >();
}


} // namespace autotime


#pragma GCC diagnostic pop

#endif // ndef AUTOTIME_OVERHEAD_IMPL_HPP

