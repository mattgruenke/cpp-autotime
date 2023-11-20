////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Container-related utilities.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_CONTAINER_UTILS_HPP
#define BENCH_CONTAINER_UTILS_HPP


#include <algorithm>
#include <cstdlib>
#include <memory>
#include <fstream>
#include <string>
#include <vector>


namespace bench
{


    //! Returns a unique, deterministic value for i.
template< typename elem_t > elem_t MakeElement( size_t i )
{
    return static_cast< elem_t >( i );
}


    //! Specialization for string - returns i-th entry of /usr/share/dict/words.
template<> std::string MakeElement< std::string >( size_t i );


    //! Returns a container populated with randomized, unique elements.
template< typename elem_t > std::shared_ptr< elem_t[] > MakeData( size_t n )
{
    srandom( 1 );   // TO_DO: make this a commandline parameter.

#if 0

    std::multimap< int, elem_t > shuffled_vals;
    for (size_t i = 0; i < n; ++i)
    {
        shuffled_vals.insert( { static_cast< int >( random() ), MakeElement< elem_t >( i ) } );
    }

#else   // Both work, but below is marginally faster than above.

    std::vector< std::pair< int, elem_t > > shuffled_vals;
    for (size_t i = 0; i < n; ++i)
    {
        shuffled_vals.push_back( { static_cast< int >( random() ), MakeElement< elem_t >( i ) } );
    }

    std::sort( shuffled_vals.begin(), shuffled_vals.end() );

#endif

    std::shared_ptr< elem_t[] > result{ new elem_t[n] };
    auto shuffled_iter = shuffled_vals.begin();
    for (size_t i = 0; i < n; ++i) result[i] = (shuffled_iter++)->second;

    return result;
}


} // namespace bench


#endif  // ndef BENCH_CONTAINER_UTILS_HPP

