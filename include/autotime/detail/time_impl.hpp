////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Low-level implementation details for Time() templates.
/*! @file

    This file is not meant to be included, directly.  See time.hpp, instead.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_DETAIL_TIME_IMPL_HPP
#define AUTOTIME_DETAIL_TIME_IMPL_HPP


namespace autotime
{


struct Durations;   // from time.hpp


namespace detail
{


class ProxyType {};


Durations TimeMember(
    void(detail::ProxyType::*)() const,
    const detail::ProxyType *inst,
    int num_iter
);


} // namespace detail


} // namespace autotime


#endif // ndef AUTOTIME_DETAIL_TIME_IMPL_HPP

