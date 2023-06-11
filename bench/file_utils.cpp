////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! File-related utilities.
/*! @file

    See file_utils.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "file_utils.hpp"


namespace bench
{


std::ifstream OpenInputFile( const std::string &filename, std::ifstream::openmode mode )
{
    std::ifstream stream;
    stream.exceptions( std::ifstream::badbit | std::ifstream::failbit );
    stream.open( filename, mode | std::ifstream::in );
    return stream;
}


} // namespace bench

