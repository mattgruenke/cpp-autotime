////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Pipe-related utility functions.
/*! @file

    See pipe_utils.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "pipe_utils.hpp"

#include "error_utils.hpp"
#include "file_utils.hpp"

#include <unistd.h>

#include <limits>
#include <stdexcept>
#include <string>

#include <fcntl.h>


namespace bench
{


void OpenPipe( int fds[2] )
{
    if (pipe( fds )) throw_system_error( errno, "pipe()" );
}


void ClosePipe( int fds[2] )
{
    for (int i = 0; i < 2; ++i)
    {
        if (fds[i] >= 0 && close( fds[i] )) throw_system_error( errno, "close()" );
        fds[i] = -1;
    }
}


size_t GetMaxPipeSize()
{
    // Cache the value for subsequent calls.
    static const int result = ReadFromFile< int >( "/proc/sys/fs/pipe-max-size" );

    if (result <= 0) throw std::runtime_error(
            "Invalid pipe max size: " + std::to_string( result ) );

    return static_cast< size_t >( result );
}


void SetPipeSize( int fds[2], size_t size )
{
    if (size > std::numeric_limits< int >::max()) throw std::out_of_range(
        "Requested size is too large even to attempt: " + std::to_string( size ) );

    int actual = fcntl( fds[1], F_SETPIPE_SZ, (int) size );
    if (actual < 0) throw_system_error( errno, "Set pipe size" );
    else if (actual < (int) size) throw std::runtime_error(
        "Set pipe size failed: " + std::to_string( actual ) + " < " + std::to_string( size ) );
}


} // namespace bench

