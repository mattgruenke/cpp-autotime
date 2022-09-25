////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Support for exception benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_EXCEPTION_UTILS_HPP
#define BENCH_EXCEPTION_UTILS_HPP

    // Disable warning of noexcept changing function pointer type signature.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnoexcept-type"


namespace bench
{


    //! Empty noexcept function.
void noexcept_func() noexcept;


    //! Calls f() inside a try block, with a catch (std::exception &).
bool catch_std(
    void (*f)()
);


    //! Calls f() inside a try block, with a catch (...).
bool catch_all(
    void (*f)()
);


    //! Calls f() with no try/catch.
bool no_catch(
    void (*f)()
);


    //! Calls f() with no try/catch.
bool no_catch_noexcept(
    void (*f)() noexcept
) noexcept;


} // namespace bench


#pragma GCC diagnostic pop

#endif  // ndef BENCH_EXCEPTION_UTILS_HPP

