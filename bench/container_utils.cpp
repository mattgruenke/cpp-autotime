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

    return words.at( i );
}


} // namespace bench


