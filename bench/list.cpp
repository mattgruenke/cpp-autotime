////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Supports benchmark enumeration and selection.
/*! @file

    See list.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "list.hpp"
#include "enum_impl.hpp"

#include <stdexcept>

#ifdef CASE
#   undef CASE
#endif


namespace bench
{


// enum class Category:
Category operator++( Category &c )
{
    c = Next< Category >( c );
    return c;
}


template<> EnumRange< Category > RangeOf< Category >()
{
    return boost::irange< Category >( Category::first, boost::next( Category::last ) );
}


const char *ToCStr( Category c )
{
    switch (c)
    {
#define CASE( n ) \
    case Category::n: \
        return #n

    CASE( atomic );
    CASE( cache );
    CASE( directory );
    CASE( file );
    CASE( function );
    CASE( heap );
    CASE( memory );
    CASE( mutex );
    CASE( network );
    CASE( process );
    CASE( socket );
    CASE( std_hashset );
    CASE( std_list );
    CASE( std_set );
    CASE( std_string );
    CASE( std_vector );
    CASE( syscall );
    CASE( thread );
    CASE( work );

#undef CASE
    }

    return nullptr;
}


std::istream &operator>>( std::istream &istream, Category &c )
{
    std::string str;
    if (istream >> str)
    {
        if (boost::optional< Category > opt = FromString< Category >( str ))
        {
            c = *opt;
            return istream;
        }
        istream.clear( std::ostream::failbit );
    }

    return istream;
}


std::ostream &operator<<( std::ostream &ostream, Category c )
{
    if (const char *c_str = ToCStr( c )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}



// enum class Benchmark:
Benchmark operator++( Benchmark &b )
{
    b = Next< Benchmark >( b );
    return b;
}


template<> EnumRange< Benchmark > RangeOf< Benchmark >()
{
    return boost::irange< Benchmark >( Benchmark::first, boost::next( Benchmark::last ) );
}


const char *ToCStr( Benchmark b )
{
    switch (b)
    {
#define CASE( n ) \
    case Benchmark::n: \
        return #n

    CASE( memcpy );
    CASE( memread );
    CASE( memset );

    CASE( fibonacci_u64 );
    CASE( mandelbrot_256 );
    CASE( trigonometry );

#undef CASE
    }

    return nullptr;
}


std::istream &operator>>( std::istream &istream, Benchmark &b )
{
    std::string str;
    if (istream >> str)
    {
        if (boost::optional< Benchmark > opt = FromString< Benchmark >( str ))
        {
            b = *opt;
            return istream;
        }
        istream.clear( std::ostream::failbit );
    }

    return istream;
}


std::ostream &operator<<( std::ostream &ostream, Benchmark b )
{
    if (const char *c_str = ToCStr( b )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}



// CategoryBenchmarkMap():
const std::map< Category, std::set< Benchmark > > &CategoryBenchmarkMap()
{
    static const std::map< Category, std::set< Benchmark > > map =
        {
            {
                Category::memory,
                {
                    Benchmark::memcpy,
                    Benchmark::memread,
                    Benchmark::memset,
                },
            },
            {
                Category::work,
                {
                    Benchmark::fibonacci_u64,
                    Benchmark::mandelbrot_256,
                    Benchmark::trigonometry,
                },
            },
        };

    return map;
}



// BenchmarkCategoryMap():
const std::map< Benchmark, Category > &BenchmarkCategoryMap()
{
    // Initializes map only on first call.
    const static auto map =
        []()
        {
            std::map< Benchmark, Category > result;
            for (auto category_benchmarks: CategoryBenchmarkMap())
            {
                for (auto benchmark: category_benchmarks.second)
                {
                    result[benchmark] = category_benchmarks.first;
                }
            }
            return result;
        }();

    return map;
}



// ParseSpecification():
std::set< Benchmark > ParseSpecification( const std::string &spec )
{
    const auto &map = CategoryBenchmarkMap();

    std::set< Benchmark > result;
    std::string::size_type ofs = 0;
    do
    {
        std::string::size_type begin = spec.find_first_not_of( ',', ofs );
        if (begin == std::string::npos) break;

        ofs = spec.find_first_of( ',', begin );
        std::string::size_type len = (ofs != std::string::npos) ? ofs - begin : std::string::npos;
        std::string name = spec.substr( begin, len );

        if (name == "all")
        {
            for (Benchmark benchmark: RangeOf< Benchmark >()) result.insert( benchmark );
        }
        else if (boost::optional< Category > cat = FromString< Category >( name ))
        {
            auto category_iter = map.find( *cat );
            if (category_iter != map.end())
            {
                const auto &benchmarks = category_iter->second;
                result.insert( benchmarks.begin(), benchmarks.end() );
            }
        }
        else if (boost::optional< Benchmark > bench = FromString< Benchmark >( name ))
        {
            result.insert( *bench );
        }
        else throw std::runtime_error( "Invalid name in spec: " + name );
    }
    while (ofs != std::string::npos);

    return result;
}



// enum class ListMode:
ListMode operator++( ListMode &mode )
{
    mode = Next< ListMode >( mode );
    return mode;
}


template<> EnumRange< ListMode > RangeOf< ListMode >()
{
    return boost::irange< ListMode >( ListMode::first, boost::next( ListMode::last ) );
}


const char *ToCStr( ListMode mode )
{
    switch (mode)
    {
    case ListMode::benchmarks:
        return "benchmarks";

    case ListMode::categories:
        return "categories";

    case ListMode::joint:
        return "joint";
    }

    return nullptr;
}


std::istream &operator>>( std::istream &istream, ListMode &mode )
{
    std::string str;
    if (istream >> str)
    {
        if (boost::optional< ListMode > opt = FromString< ListMode >( str ))
        {
            mode = *opt;
            return istream;
        }
        istream.clear( std::ostream::failbit );
    }

    return istream;
}


std::ostream &operator<<( std::ostream &ostream, ListMode mode )
{
    if (const char *c_str = ToCStr( mode )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}



// Print():
std::ostream &PrintList(
    std::ostream &ostream, const std::set< Benchmark > &list, ListMode mode )
{
    switch (mode)
    {
    case ListMode::benchmarks:
        for (auto benchmark: list) ostream << benchmark << "\n";
        break;

    case ListMode::categories:
        {
            const auto &category_of = BenchmarkCategoryMap();
            std::set< Category > categories;
            for (auto benchmark: list)
            {
                auto iter = category_of.find( benchmark );
                if (iter != category_of.end()) categories.insert( iter->second );
            }
            for (auto category: categories) ostream << category << "\n";
        }
        break;

    case ListMode::joint:
        {
            const auto &category_of = BenchmarkCategoryMap();
            std::map< Category, std::set< Benchmark > > by_category;
            for (auto benchmark: list)
            {
                auto iter = category_of.find( benchmark );
                if (iter != category_of.end()) by_category[iter->second].insert( benchmark );
            }

            for (auto category_benchmarks: by_category)
            {
                ostream << category_benchmarks.first << ":\n";
                for (auto benchmark: category_benchmarks.second)
                {
                    ostream << "  " << benchmark << "\n";
                }
            }
        }
        break;
    }

    return ostream;
}


} // namespace bench

