////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Provides descriptive information about benchmarks and categories.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_DESCRIPTION_HPP
#define BENCH_DESCRIPTION_HPP


#include <set>
#include <string>
#include <vector>

#include "list.hpp"


namespace bench
{


    //! Encapsulates a textual description of a benchmark or category.
    /*!
        The string values in this class may include markdown syntax.
    */
struct Description
{
    std::string measures;               //!< What's measured (briefly).
    std::string overhead;               //!< What's excluded from measurement.
    std::string detail;                 //!< Provides all relevant details.
    std::vector< std::string > notes;   //!< Attention-worthy details.
    std::vector< std::string > limits;  //!< What isn't or can't be measured.
    std::vector< std::string > to_dos;  //!< Remaining work or improvements.
};


    //! Returns the description of a benchmark determined at runtime.
Description Describe( Benchmark b );


    //! Returns the description of a category determined at runtime.
Description Describe( Category c );


    //! Returns the description of a benchmark determined at compile-time.
template<
    Benchmark b
>
Description Describe();


    //! Returns the description of a category determined at compile-time.
template<
    Category c
>
Description Describe();


    //! Prints the descriptions of benchmarks/categories to an ostream.
std::ostream &PrintDescriptions(
    std::ostream &ostream,
    const std::set< Benchmark > &list,
    ListMode mode
);


} // namespace bench


#endif // ndef BENCH_DESCRIPTION_HPP

