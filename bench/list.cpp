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
    CASE( condvar );
    CASE( exception );
    CASE( directory );
    CASE( file );
    CASE( function );
    CASE( heap );
    CASE( istream );
    CASE( memory );
    CASE( mutex );
    CASE( network );
    CASE( ostream );
    CASE( overhead );
    CASE( process );
    CASE( socket );
    CASE( std_hashset );
    CASE( std_list );
    CASE( std_set );
    CASE( std_string );
    CASE( std_vector );
    CASE( string_from );
    CASE( string_to );
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

    CASE( atomic_increment );
    CASE( atomic_cmp_exchg_weak_xpctd );
    CASE( atomic_cmp_exchg_weak_other );
    CASE( atomic_cmp_exchg_strong_xpctd );
    CASE( atomic_cmp_exchg_strong_other );
    CASE( atomic_pingpong );

    CASE( condvar_broadcast );
    CASE( condvar_create_destroy );
    CASE( condvar_signal );
    CASE( condvar_signal_watched );

    CASE( exception_catch_all );
    CASE( exception_catch_std );
    CASE( exception_noexcept );
    CASE( exception_try );

    CASE( function_args0 );
    CASE( function_args1 );
    CASE( function_args2 );
    CASE( function_args3 );
    CASE( function_args4 );
    CASE( function_args5 );
    CASE( function_args6 );
    CASE( function_args7 );
    CASE( member_function );
    CASE( member_function_shptr );
    CASE( virtual_function );

    CASE( new_delete_16 );
    CASE( new_delete_256 );
    CASE( new_delete_4k );
    CASE( new_delete_64k );
    CASE( new_delete_1M );
    CASE( new_delete_16M );
    CASE( new_delete_256M );
    CASE( new_delete_cold_16 );
    CASE( new_delete_cold_4k );
    CASE( new_delete_cold_1M );
    CASE( new_delete_cold_256M );

    CASE( istream_string4 );
    CASE( istream_string64 );
    CASE( istream_smallint );
    CASE( istream_maxint );
    CASE( istream_maxint64 );
    CASE( istream_smallfloat );
    CASE( istream_bigfloat );
    CASE( istream_smalldouble );
    CASE( istream_bigdouble );

    CASE( memcpy_256  );
    CASE( memcpy_4k   );
    CASE( memcpy_64k  );
    CASE( memcpy_1M   );
    CASE( memcpy_16M  );
    CASE( memcpy_256M );
    CASE( strlen_256  );
    CASE( strlen_4k   );
    CASE( strlen_64k  );
    CASE( strlen_1M   );
    CASE( strlen_16M  );
    CASE( strlen_256M );
    CASE( memset_256  );
    CASE( memset_4k   );
    CASE( memset_64k  );
    CASE( memset_1M   );
    CASE( memset_16M  );
    CASE( memset_256M );

    CASE( mutex_create_destroy );
    CASE( mutex_lock_release );
    CASE( mutex_trylock );

    CASE( ostream_string4 );
    CASE( ostream_string64 );
    CASE( ostream_cstr4 );
    CASE( ostream_cstr64 );
    CASE( ostream_smallint );
    CASE( ostream_maxint );
    CASE( ostream_maxint64 );
    CASE( ostream_smallfloat );
    CASE( ostream_bigfloat );
    CASE( ostream_smalldouble );
    CASE( ostream_bigdouble );
    CASE( ostream_endl );

    CASE( overhead_nullary );
    CASE( overhead_nullary_fn );
    CASE( overhead_nullary_ptr );

    CASE( string_from_smallint );
    CASE( string_from_maxint );
    CASE( string_from_maxint64 );
    CASE( string_from_smallfloat );
    CASE( string_from_bigfloat );
    CASE( string_from_smalldouble );
    CASE( string_from_bigdouble );

    CASE( string_to_smallint );
    CASE( string_to_maxint );
    CASE( string_to_maxint64 );
    CASE( string_to_smallfloat_os );
    CASE( string_to_smallfloat_ts );
    CASE( string_to_bigfloat_os );
    CASE( string_to_bigfloat_ts );
    CASE( string_to_smalldouble_os );
    CASE( string_to_smalldouble_ts );
    CASE( string_to_bigdouble_os );
    CASE( string_to_bigdouble_ts );

    CASE( thread_create_destroy );
    CASE( thread_pingpong );

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
                Category::atomic,
                {
                    Benchmark::atomic_increment,
                    Benchmark::atomic_cmp_exchg_weak_xpctd,
                    Benchmark::atomic_cmp_exchg_weak_other,
                    Benchmark::atomic_cmp_exchg_strong_xpctd,
                    Benchmark::atomic_cmp_exchg_strong_other,
                    Benchmark::atomic_pingpong,
                },
            },
            {
                Category::condvar,
                {
                    Benchmark::condvar_broadcast,
                    Benchmark::condvar_create_destroy,
                    Benchmark::condvar_signal,
                    Benchmark::condvar_signal_watched,
                },
            },
            {
                Category::exception,
                {
                    Benchmark::exception_catch_all,
                    Benchmark::exception_catch_std,
                    Benchmark::exception_noexcept,
                    Benchmark::exception_try,
                },
            },
            {
                Category::function,
                {
                    Benchmark::function_args0,
                    Benchmark::function_args1,
                    Benchmark::function_args2,
                    Benchmark::function_args3,
                    Benchmark::function_args4,
                    Benchmark::function_args5,
                    Benchmark::function_args6,
                    Benchmark::function_args7,
                    Benchmark::member_function,
                    Benchmark::member_function_shptr,
                    Benchmark::virtual_function,
                },
            },
            {
                Category::heap,
                {
                    Benchmark::new_delete_16,
                    Benchmark::new_delete_256,
                    Benchmark::new_delete_4k,
                    Benchmark::new_delete_64k,
                    Benchmark::new_delete_1M,
                    Benchmark::new_delete_16M,
                    Benchmark::new_delete_256M,
                    Benchmark::new_delete_cold_16,
                    Benchmark::new_delete_cold_4k,
                    Benchmark::new_delete_cold_1M,
                    Benchmark::new_delete_cold_256M,
                },
            },
            {
                Category::istream,
                {
                    Benchmark::istream_string4,
                    Benchmark::istream_string64,
                    Benchmark::istream_smallint,
                    Benchmark::istream_maxint,
                    Benchmark::istream_maxint64,
                    Benchmark::istream_smallfloat,
                    Benchmark::istream_bigfloat,
                    Benchmark::istream_smalldouble,
                    Benchmark::istream_bigdouble,
                },
            },
            {
                Category::memory,
                {
                    Benchmark::memcpy_256,
                    Benchmark::memcpy_4k,
                    Benchmark::memcpy_64k,
                    Benchmark::memcpy_1M,
                    Benchmark::memcpy_16M,
                    Benchmark::memcpy_256M,
                    Benchmark::strlen_256,
                    Benchmark::strlen_4k,
                    Benchmark::strlen_64k,
                    Benchmark::strlen_1M,
                    Benchmark::strlen_16M,
                    Benchmark::strlen_256M,
                    Benchmark::memset_256,
                    Benchmark::memset_4k,
                    Benchmark::memset_64k,
                    Benchmark::memset_1M,
                    Benchmark::memset_16M,
                    Benchmark::memset_256M,
                },
            },
            {
                Category::mutex,
                {
                    Benchmark::mutex_create_destroy,
                    Benchmark::mutex_lock_release,
                    Benchmark::mutex_trylock,
                },
            },
            {
                Category::ostream,
                {
                    Benchmark::ostream_string4,
                    Benchmark::ostream_string64,
                    Benchmark::ostream_cstr4,
                    Benchmark::ostream_cstr64,
                    Benchmark::ostream_smallint,
                    Benchmark::ostream_maxint,
                    Benchmark::ostream_maxint64,
                    Benchmark::ostream_smallfloat,
                    Benchmark::ostream_bigfloat,
                    Benchmark::ostream_smalldouble,
                    Benchmark::ostream_bigdouble,
                    Benchmark::ostream_endl,
                },
            },
            {
                Category::overhead,
                {
                    Benchmark::overhead_nullary,
                    Benchmark::overhead_nullary_fn,
                    Benchmark::overhead_nullary_ptr,
                },
            },
            {
                Category::string_from,
                {
                    Benchmark::string_from_smallint,
                    Benchmark::string_from_maxint,
                    Benchmark::string_from_maxint64,
                    Benchmark::string_from_smallfloat,
                    Benchmark::string_from_bigfloat,
                    Benchmark::string_from_smalldouble,
                    Benchmark::string_from_bigdouble,
                },
            },
            {
                Category::string_to,
                {
                    Benchmark::string_to_smallint,
                    Benchmark::string_to_maxint,
                    Benchmark::string_to_maxint64,
                    Benchmark::string_to_smallfloat_os,
                    Benchmark::string_to_smallfloat_ts,
                    Benchmark::string_to_bigfloat_os,
                    Benchmark::string_to_bigfloat_ts,
                    Benchmark::string_to_smalldouble_os,
                    Benchmark::string_to_smalldouble_ts,
                    Benchmark::string_to_bigdouble_os,
                    Benchmark::string_to_bigdouble_ts,
                },
            },
            {
                Category::thread,
                {
                    Benchmark::thread_create_destroy,
                    Benchmark::thread_pingpong,
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
                ostream << "\n";
            }
        }
        break;
    }

    return ostream;
}


} // namespace bench

