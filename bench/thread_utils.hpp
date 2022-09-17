////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! Threading-related utilities.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_THREAD_UTILS_HPP
#define BENCH_THREAD_UTILS_HPP

namespace bench
{


    //! Creates a thread, just to ensure libpthread is fully initialized.
void *InitLibpthread();


    //! Returns the ID of the secondary core to use for threaded benchmarks.
int GetSecondaryCoreId();


    //! Returns the ID of the secondary core to use for threaded benchmarks.
void SetSecondaryCoreId(
    int id      //!< Core ID (-1 for current core).
);


} // namespace bench


#endif  // ndef BENCH_THREAD_UTILS_HPP

