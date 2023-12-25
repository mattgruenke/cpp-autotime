////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements output formatting utilities.
/*! @file

    See output.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "output.hpp"
#include "enum_impl.hpp"
#include "format_utils.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>


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


std::ostream &PrettyPrintSizeof( std::ostream &ostream, size_t size )
{
    if (size == 0) return ostream << size << " ";

    const std::vector< std::string > prefixes = { "", "ki", "Mi", "Gi", "Ti" };
    double exp = log2( size );
    long int prefix_idx = lrint( std::min< double >( floor( exp / 10 ), prefixes.size() - 1 ) );
    double scale = exp2( prefix_idx * 10 );
    double scaled = size / scale;
    if (floor( scaled ) == scaled) ostream << lrint( scaled );
    else ostream << scaled;
    return ostream << " " << prefixes.at( prefix_idx );
}


static std::ostream &PrettyPrint( std::ostream &ostream, const NormDurations::duration &d )
{
    const std::vector< char > prefixes = { 'n', 'u', 'm' };
    double ns = d.count() / 1000.0;
    double exp = log10( std::max( ns, 0.0 ) );
    long int prefix_idx = lrint( std::min( std::max( floor( exp / 3 ), 0.0 ), 2.0 ) );
    double scale = exp10( prefix_idx * 3 );
    return ostream << (ns / scale) << " " << prefixes.at( prefix_idx ) << "s";
}


void PrettyOutputFormatter::write(
    Benchmark benchmark, NormDurations norm, int num_iters, CpuClockPeriod )
{
    const auto precision_prev = ostream_.precision( 4 );
    ostream_ << benchmark << ": "
        << "{ ";
    PrettyPrint( ostream_, norm.real ) << ", ";
    PrettyPrint( ostream_, norm.thread ) << " }";
    ostream_ << " in " << num_iters << " iters\n";
    ostream_.precision( precision_prev );
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

