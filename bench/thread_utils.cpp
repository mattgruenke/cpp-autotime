////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Threading-related utilities.
/*! @file

    See thread_utils.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "thread_utils.hpp"

#include <system_error>

#include <pthread.h>


namespace bench
{


void *InitLibpthread()
{
    static void *result = []()
        {
            pthread_t thread{};
            int ev = pthread_create( &thread, nullptr,
                []( void * ) -> void * { return nullptr; }, nullptr );
            if (ev) throw std::system_error( ev, std::system_category(), "pthread_create()" );

            void *result = nullptr;
            ev = pthread_join( thread, &result );
            if (ev) throw std::system_error( ev, std::system_category(), "pthread_join()" );

            return result;
        }();

    return result;
}


} // namespace bench

