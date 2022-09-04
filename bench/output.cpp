////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Output utilities.
/*! @file

    See output.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "output.hpp"
#include "enum_impl.hpp"


namespace bench
{


Format operator++( Format &f )
{
    f = Next< Format >( f );
    return f;
}


template<> EnumRange< Format > RangeOf< Format >()
{
    Format begin = Format::pretty;
    Format end = boost::next( Format::csv );
    return boost::irange< Format >( begin, end );
}


const char *ToString( Format f )
{
    switch (f)
    {
    case Format::pretty:
        return "pretty";

    case Format::csv:
        return "CSV";
    }

    return nullptr;
}


std::istream &operator>>( std::istream &istream, Format &f )
{
    std::string str;
    if (istream >> str)
    {
        if (boost::optional< Format > opt = FromString< Format >( str ))
        {
            f = *opt;
            return istream;
        }
        istream.clear( std::ostream::failbit );
    }

    return istream;
}


std::ostream &operator<<( std::ostream &ostream, Format f )
{
    if (const char *c_str = ToString( f )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}


} // namespace bench

