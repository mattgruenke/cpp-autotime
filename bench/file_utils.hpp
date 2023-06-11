////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! File-related utilities.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_FILE_UTILS_HPP
#define BENCH_FILE_UTILS_HPP


#include <fstream>
#include <string>


namespace bench
{


    //! Default supplementary mode bits value (none).
static constexpr std::ios_base::openmode OpenMode0{};


    //! Opens a file for input, enabling std::ios_base::failure exceptions.
    /*!
        @throws std::ios_base::failure if file couldn't be opened for input.

        The mode parameter is for specifying bits besides std::ios_base::in,
        if desired.  Typically, this would just be std::ios_base::binary.
    */
std::ifstream OpenInputFile(
    const std::string &filename,                //!< Absolute or relative path.
    std::ifstream::openmode mode = OpenMode0    //!< Additional mode bits.
);


    //! Reads a single value from the specified file.
    /*!
        @throws std::ios_base::failure
            if file couldn't be opened for input or read fails.
    */
template<
    typename value_type
>
value_type ReadFromFile(
    const std::string &filename                 //!< Absolute or relative path.
)
{
    value_type value;
    std::ifstream stream{ OpenInputFile( filename ) };
    stream >> value;
    return value;
}


} // namespace bench


#endif  // ndef BENCH_FILE_UTILS_HPP

