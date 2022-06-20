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


} // namespace autotime


#endif // ndef AUTOTIME_WORK_HPP

