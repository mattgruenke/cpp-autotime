////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Pipe-related utility functions.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_PIPE_UTILS_HPP
#define BENCH_PIPE_UTILS_HPP


#include <cstddef>


namespace bench
{


    //! Opens a new pipe.
    /*!
        @throws std::system_error if the pipe() fails.
    */
void OpenPipe( int fds[2] );


    //! Closes an existing pipe.
    /*!
        @throws std::system_error if the close() fails.
    */
void ClosePipe( int fds[2] );


    //! Returns the system's maximum supported pipe buffer size.
size_t GetMaxPipeSize();


    //! Attempts to set the buffer size of a pipe.
    /*!
        @throws std::out_of_range if the requested size is too large to attempt.

        @throws std::system_error if the fcntl() fails.

        @throws std::runtime_error if the actual size is less than requested.
    */
void SetPipeSize(
    int fds[2],
    size_t size
);


} // namespace bench


#endif  // ndef BENCH_PIPE_UTILS_HPP

