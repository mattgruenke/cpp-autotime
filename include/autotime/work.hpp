////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Declares various busy functions.
/*! @file

    For testing, among other purposes, it's useful to have a variety of ways to
    burn CPU cycles.
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef AUTOTIME_WORK_HPP
#define AUTOTIME_WORK_HPP


#include <complex>


namespace autotime
{


    //! Calls a sequence of trigonometry functions on an unbounded parameter.
    /*!
        @returns a value similar to the input.
    */
float TrigonometryFunctions( float tangent );


    //! Computes the largest Fibonacci number that fits a given datatype.
    /*!
        Instantiated for T: { uint32_t, uint64_t }.
    */
template< typename T > T LargestFibonacci();


    //! Attempts to determine membership of a given number in Mandlebrot set.
    /*!
        @returns number of iters needed to reject the parameter; -1 otherwise.
    */
int Mandelbrot( std::complex< float > c, int max_iters );


} // namespace autotime


#endif // ndef AUTOTIME_WORK_HPP

