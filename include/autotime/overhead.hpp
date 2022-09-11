////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Provides generic mechanisms for approximating function call overhead.
/*! @file

    See also: overhead_impl.hpp.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_OVERHEAD_HPP
#define AUTOTIME_OVERHEAD_HPP


#include <functional>


namespace autotime
{


    //! Generic function for approximating function call overhead.
    /*!
        To use this, ensure that an instantiation exists in a different
        translation unit than where you're calling it.
        See overhead_impl.hpp, for details.
    */
template<
    typename return_t,
    typename... params_t
>
return_t Overhead(
    params_t...
);


    //! Variant of Overhead<>() for functions returning void.
    /*!
        See Overhead<>(), for details.
    */
template<
    typename... params_t
>
void Overhead_void(
    params_t...
);


    //! Factory for constructing empty std::function<> instances.
    /*!
        Since std::function<> seems to have different characteristics
        when assigned directly from a lambda, use this if that's what
        you ultimately want.
    */
template<
    typename return_t,
    typename... params_t
>
std::function< return_t( params_t... ) > MakeOverheadFn();


    //! Factory for generating function pointers to empty lambdas.
    /*!
        In case pointers to lambdas behave differently than &Overhead<>.
    */
template<
    typename return_t,
    typename... params_t
>
return_t(*MakeOverheadPtr( params_t... ))();


} // namespace autotime


#endif // ndef AUTOTIME_OVERHEAD_HPP

