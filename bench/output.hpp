////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Output utilities.
/*! @file

    Utility functions & types to format benchmark results.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_OUTPUT_HPP
#define BENCH_OUTPUT_HPP


#include <iosfwd>
#include <memory>

#include "enum_utils.hpp"
#include "list.hpp"

#include "autotime/types.hpp"


namespace bench
{


enum class Format
{
    pretty, first = pretty,
    csv,    last = csv
};

Format operator++( Format &f );

template<> EnumRange< Format > RangeOf< Format >();

const char *ToCStr( Format f );

std::istream &operator>>( std::istream &istream, Format &f );
std::ostream &operator<<( std::ostream &ostream, Format f );


    //! Output formatting interface.
class IOutputFormatter
{
public:
        //! Creates corresponding instance & prints header (if applicable).
    static std::unique_ptr< IOutputFormatter > create(
        std::ostream &ostream,
        Format format );

        //! Prints footer (if applicable).
    virtual ~IOutputFormatter();

        //! Writes a single result.
    virtual void write(
        Benchmark benchmark,
        autotime::NormDurations norm,
        int num_iters,
        autotime::CpuClockPeriod clockspeed
    ) = 0;
};


} // namespace bench


#endif  // ndef BENCH_OUTPUT_HPP

