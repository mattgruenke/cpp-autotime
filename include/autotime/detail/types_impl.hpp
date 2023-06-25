////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Low-level implementation types.
/*! @file

    This file is not meant to be included, directly.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_DETAIL_TYPES_IMPL_HPP
#define AUTOTIME_DETAIL_TYPES_IMPL_HPP


#define AUTOTIME_DETAIL_NO_INLINE __attribute__((noinline))


namespace autotime
{


namespace detail
{


    // Used by non-inline functions which accept pointer-to-member parameters.
class ProxyType {};


} // namespace detail


} // namespace autotime


#endif // ndef AUTOTIME_DETAIL_TYPES_IMPL_HPP

