////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Low-level implementation details for custom clocks.
/*! @file

    This file is not meant to be included, directly.  See clocks.hpp, instead.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_DETAIL_CLOCK_IMPL_HPP
#define AUTOTIME_DETAIL_CLOCK_IMPL_HPP


#include <ctime>


namespace autotime
{


namespace detail
{


template<
    class clock,
    clockid_t clk_id
>
inline typename clock::time_point now() noexcept
{
    timespec t{};
    clock_gettime( clk_id, &t );

    using rep = typename clock::rep;
    return typename clock::time_point{
        typename clock::duration{
            1000000000 * static_cast< rep >( t.tv_sec ) + t.tv_nsec } };
}


} // namespace detail


} // namespace autotime


#endif // ndef AUTOTIME_DETAIL_CLOCK_IMPL_HPP

