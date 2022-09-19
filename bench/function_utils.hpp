////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Support for function benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_FUNCTION_UTILS_HPP
#define BENCH_FUNCTION_UTILS_HPP

#include <memory>


namespace bench
{


template<
    typename return_t,
    typename... params_t
>
return_t Function(
    params_t...
);


struct Base
{
    virtual ~Base();
    virtual void vmethod() = 0;

    static Base &get();
};

void CallVMethod( Base &base );

void CallVMethod( const std::shared_ptr< Base > &p_base );

void CallNothing( Base &base );

void CallNothing( const std::shared_ptr< Base > &p_base );


struct Owner final: public Base
{
    void method();
    void vmethod() override;
};


} // namespace bench


#endif  // ndef BENCH_FUNCTION_UTILS_HPP

