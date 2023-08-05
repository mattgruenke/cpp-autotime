////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements support for querying and printing descriptive information about benches & cats.
/*! @file

    See description.hpp, for more information.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "description.hpp"


namespace bench
{


Description Describe( Benchmark benchmark )
{
    switch (benchmark)
    {
#define CASE__DESCRIBE( n ) \
    case Benchmark::n: \
        return Describe< Benchmark::n >()

    CASE__DESCRIBE( asio_reset );
    CASE__DESCRIBE( asio_run );

#undef CASE__DESCRIBE

    default:
        break;
    }

    return {};
}


Description Describe( Category category )
{
    switch (category)
    {
#define CASE__DESCRIBE( n ) \
    case Category::n: \
        return Describe< Category::n >()

    CASE__DESCRIBE( asio );

#undef CASE__DESCRIBE

    default:
        break;
    }

    return {};
}


std::ostream &PrintOneliner(
    std::ostream &ostream, const std::string &name, const std::string &value )
{
    return ostream << name << ": " << value;
}


std::ostream &PrintParagraph(
    std::ostream &ostream,
    const std::string &name, const std::string &value, const std::string &indent )
{
    return ostream << indent << name << ":\n\n  " << indent << value;
}


std::ostream &PrintParagraphs(
    std::ostream &ostream,
    const std::string &name, const std::vector< std::string > &values, const std::string &indent )
{
    ostream << indent << name << ":\n";
    for (const std::string &value: values) ostream << "\n  " << indent << value << "\n";
    return ostream;
}


std::ostream &PrintDescription(
    std::ostream &os, const Description &d, const std::string &indent )
{
    if (!d.measures.empty()) PrintOneliner( os << indent, "Measures", d.measures ) << "\n";
    if (!d.overhead.empty()) PrintOneliner( os << indent, "Excludes", d.overhead ) << "\n";
    if (!d.detail.empty()) PrintParagraph( os, "Details", d.detail, indent ) << "\n";
    if (!d.notes.empty()) PrintParagraphs( os, "Notes", d.notes, indent ) << "\n";
    if (!d.limits.empty()) PrintParagraphs( os, "Limitations", d.limits, indent ) << "\n";
    if (!d.to_dos.empty()) PrintParagraphs( os, "To Do", d.to_dos, indent ) << "\n";

    return os;
}


std::ostream &PrintDescriptions(
    std::ostream &ostream, const std::set< Benchmark > &list, ListMode mode )
{
    switch (mode)
    {
    case ListMode::benchmarks:
        for (auto benchmark: list)
        {
            ostream << ToCStr( benchmark ) << "\n";
            PrintDescription( ostream, Describe( benchmark ), "\n  " ) << "\n";
        }
        break;

    case ListMode::categories:
        {
            const std::set< Category > categories = CategoriesOf( list );
            for (auto category: categories)
            {
                ostream << ToCStr( category ) << "\n";
                PrintDescription( ostream, Describe( category ), "\n  " ) << "\n";
            }
        }
        break;

    case ListMode::joint:
        {
            const std::map< Category, std::set< Benchmark > > by_category = GroupByCategory( list );
            for (auto category_benchmarks: by_category)
            {
                const Category category = category_benchmarks.first;
                ostream << ToCStr( category ) << "\n";
                PrintDescription( ostream, Describe( category ), "\n  " ) << "\n";

                ostream << "  Benchmarks:\n\n";
                for (auto benchmark: category_benchmarks.second)
                {
                    ostream << "    " << ToCStr( benchmark ) << "\n";
                    PrintDescription( ostream, Describe( benchmark ), "\n      " ) << "\n";
                }
                ostream << "\n";
            }
        }
        break;
    }

    return ostream;
}


} // namespace bench

