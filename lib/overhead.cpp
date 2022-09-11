////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Instantiates a few common specializations of Overhead-family templates.
/*! @file

    See overhead.hpp and overhead_impl.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "autotime/overhead_impl.hpp"


namespace autotime
{


template void Overhead_void<>();

template std::function< void() > MakeOverheadFn< void >();

template void(*MakeOverheadPtr< void >())();


} // namespace autotime

