////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Formatting-related utilities.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_FORMAT_UTILS_HPP
#define BENCH_FORMAT_UTILS_HPP

#include <iosfwd>
#include <string>
#include <type_traits>

#include <boost/mpl/if.hpp>


namespace bench
{


    //! Pretty prints a size with the closest power-of-2 suffix.
std::ostream &PrettyPrintSizeof( std::ostream &ostream, size_t size );


    //! Internal helper type for PrettyTypename().
template< bool is_signed, size_t size_of > struct IntegralNameBuilder;


template< size_t size_of >
    struct IntegralNameBuilder< true, size_of >
{
    static std::string pretty()
    {
        return "int" + std::to_string( size_of * 8 );
    }
};


template< size_t size_of >
    struct IntegralNameBuilder< false, size_of >
{
    static std::string pretty()
    {
        return "u" + IntegralNameBuilder< true, size_of >::pretty();
    }
};


    //! Internal helper type for PrettyTypename().
template< typename T >
    struct IntegralNamer
{
    static std::string pretty()
    {
        constexpr bool is_signed = std::is_signed< T >::type::value;
        return IntegralNameBuilder< is_signed, sizeof( T ) >::pretty();
    }
};


    //! Internal helper type for PrettyTypename().
template< typename T > struct GenericNamer;


template<> struct GenericNamer< float >
{
    static std::string pretty()
    {
        return "float";
    }
};


template<> struct GenericNamer< double >
{
    static std::string pretty()
    {
        return "double";
    }
};


template<> struct GenericNamer< std::string >
{
    static std::string pretty()
    {
        return "std::string";
    }
};


    //! Returns a type's pretty name.
template<
    typename T  //!< Type to name.
>
std::string PrettyTypename()
{
    constexpr bool is_integral = std::is_integral< T >::type::value;
    using namer_type = typename boost::mpl::if_c< is_integral,
            IntegralNamer< T >, GenericNamer< T > >::type;
    return namer_type::pretty();
}


} // namespace bench


#endif  // ndef BENCH_FORMAT_UTILS_HPP

