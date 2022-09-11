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

#include <stdexcept>


using namespace autotime;


namespace bench
{


class PrettyOutputFormatter final: public IOutputFormatter
{
public:
    explicit PrettyOutputFormatter( std::ostream &ostream );
    void write( Benchmark, NormDurations, int, CpuClockPeriod ) override;

private:
    std::ostream &ostream_;
};



// enum class Format:
Format operator++( Format &f )
{
    f = Next< Format >( f );
    return f;
}


template<> EnumRange< Format > RangeOf< Format >()
{
    return boost::irange< Format >( Format::first, boost::next( Format::last ) );
}


const char *ToCStr( Format f )
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
    if (const char *c_str = ToCStr( f )) ostream << c_str;
    else ostream.clear( std::ostream::failbit );

    return ostream;
}



// class PrettyOutputFormatter:
PrettyOutputFormatter::PrettyOutputFormatter( std::ostream &ostream )
:
    ostream_( ostream )
{
}


void PrettyOutputFormatter::write(
    Benchmark benchmark, NormDurations norm, int num_iters, CpuClockPeriod )
{
    double real_ns   = norm.real.count()   / 1000.0;
    double thread_ns = norm.thread.count() / 1000.0;
    ostream_ << benchmark << ": "
        << "{ " << real_ns << " ns, " << thread_ns << " ns }"
        << " in " << num_iters << " iters\n";
}



// class IOutputFormatter:
std::unique_ptr< IOutputFormatter > IOutputFormatter::create(
    std::ostream &ostream, Format format )
{
    switch (format)
    {
    case Format::pretty:
        return std::unique_ptr< PrettyOutputFormatter >( new PrettyOutputFormatter( ostream ) );

    case Format::csv:
        break;
    }

    throw std::runtime_error( "Unsupported output format: " + ToStr( format ) );
}


IOutputFormatter::~IOutputFormatter()
{
}


} // namespace bench

