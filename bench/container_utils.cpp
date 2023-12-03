////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Container-related utilities.
/*! @file

    See container_utils.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "container_utils.hpp"


namespace bench
{


template<> std::string MakeElement< std::string >( size_t i )
{
    static std::vector< std::string > words = []()
        {
            std::vector< std::string > result;
            std::ifstream file( "/usr/share/dict/words" );  // TO_DO: make this a CLI parameter.
            std::string word;
            while (std::getline( file,word )) result.push_back( word );
            return result;
        }();

    return words[i % words.size()];
}


std::shared_ptr< std::string[] > MakeStringData( size_t len, size_t n )
{
    std::shared_ptr< std::string[] > result{ new std::string[n] };

    std::ifstream file( "/usr/share/dict/words" );  // TO_DO: make this a CLI parameter.
    std::string read_buf( len, ' ' );
    for (size_t i = 0; i < n; ++i)
    {
        size_t got = 0;
        while (got < len)
        {
            got += file.readsome( const_cast< char * >( read_buf.data() + got ), len - got );
            if (got < len) file.seekg( 0 );
        }

        result[i] = read_buf;
    }

    return result;
}


} // namespace bench


