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

    CASE__DESCRIBE( chmod );
    CASE__DESCRIBE( chown );
    CASE__DESCRIBE( directory_iter_0 );
    CASE__DESCRIBE( directory_iter_1 );
    CASE__DESCRIBE( directory_iter_64 );
    CASE__DESCRIBE( directory_iter_4k );
    CASE__DESCRIBE( flock );
    CASE__DESCRIBE( inotify );
    CASE__DESCRIBE( mkdir );
    CASE__DESCRIBE( readdir_0 );
    CASE__DESCRIBE( readdir_1 );
    CASE__DESCRIBE( readdir_64 );
    CASE__DESCRIBE( readdir_4k );
    CASE__DESCRIBE( rmdir );

    CASE__DESCRIBE( file_append );
    CASE__DESCRIBE( file_close );
    CASE__DESCRIBE( file_create );
    CASE__DESCRIBE( file_fdatasync_0 );
    CASE__DESCRIBE( file_fdatasync_1 );
    CASE__DESCRIBE( file_fdatasync_block );
    CASE__DESCRIBE( file_fsync_0 );
    CASE__DESCRIBE( file_fsync_1 );
    CASE__DESCRIBE( file_fsync_block );
    CASE__DESCRIBE( file_lseek_random );
    CASE__DESCRIBE( file_open_nonexistent );
    CASE__DESCRIBE( file_open_ro );
    CASE__DESCRIBE( file_open_rw );
    CASE__DESCRIBE( file_open_trunc );
    CASE__DESCRIBE( file_read_256 );
    CASE__DESCRIBE( file_read_4k );
    CASE__DESCRIBE( file_read_64k );
    CASE__DESCRIBE( file_read_1M );
    CASE__DESCRIBE( file_read_16M );
    CASE__DESCRIBE( file_read_direct_4k );
    CASE__DESCRIBE( file_read_direct_64k );
    CASE__DESCRIBE( file_read_direct_1M );
    CASE__DESCRIBE( file_read_direct_16M );
    CASE__DESCRIBE( file_stat );
    CASE__DESCRIBE( file_stat_nonexistent );
    CASE__DESCRIBE( file_unlink );
    CASE__DESCRIBE( file_write_256 );
    CASE__DESCRIBE( file_write_4k );
    CASE__DESCRIBE( file_write_64k );
    CASE__DESCRIBE( file_write_1M );
    CASE__DESCRIBE( file_write_16M );
    CASE__DESCRIBE( file_write_direct_4k );
    CASE__DESCRIBE( file_write_direct_64k );
    CASE__DESCRIBE( file_write_direct_1M );
    CASE__DESCRIBE( file_write_direct_16M );

    CASE__DESCRIBE( pipe_open_close );

    CASE__DESCRIBE( hash_int8 );
    CASE__DESCRIBE( hash_int16 );
    CASE__DESCRIBE( hash_int32 );
    CASE__DESCRIBE( hash_int64 );
    CASE__DESCRIBE( hash_float );
    CASE__DESCRIBE( hash_double );
    CASE__DESCRIBE( hash_string_1 );
    CASE__DESCRIBE( hash_string_16 );
    CASE__DESCRIBE( hash_string_256 );
    CASE__DESCRIBE( hash_string_4k );
    CASE__DESCRIBE( hash_string_64k );

    CASE__DESCRIBE( cache_false_sharing );

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
    CASE__DESCRIBE( directory );
    CASE__DESCRIBE( file );
    CASE__DESCRIBE( pipe );
    CASE__DESCRIBE( std_hash );

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
    return ostream << indent << name << ":\n  " << indent << value;
}


std::ostream &PrintParagraphs(
    std::ostream &ostream,
    const std::string &name, const std::vector< std::string > &values, const std::string &indent )
{
    ostream << indent << name << ":\n";
    for (const std::string &value: values) ostream << indent << "  " << value << "\n";
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

