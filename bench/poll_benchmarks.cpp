////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements benchmarks of file descriptor monitoring functions.
/*! @file

    See Category::poll, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>  // Don't forget to #undef NDEBUG, above, if you want assert() to work.
#include <memory>
#include <vector>

#include <poll.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/select.h>

#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "description.hpp"
#include "dispatch.hpp"
#include "error_utils.hpp"
#include "file_utils.hpp"
#include "list.hpp"
#include "format_utils.hpp"


using namespace autotime;

namespace bench
{


template<> Description Describe< Category::poll >()
{
    Description desc;
    desc.measures = "File descriptor monitoring functions.";
    return desc;
}


static int MakeEventFd()
{
    int fd = eventfd( 0, 0 );
    if (fd < 0) throw_system_error( errno, "eventfd()" );
    return fd;
}


static std::shared_ptr< std::vector< ScopedFile > > MakeEventFds( size_t num )
{
    auto p_files = std::make_shared< std::vector< ScopedFile > >();
    for (size_t i = 0; i < num; ++i)
    {
        p_files->emplace_back( ScopedFile::make_bound( MakeEventFd() ) );
    }

    return p_files;
}


template<
    size_t size
>
static Description DescribeEpoll()
{
    std::ostringstream oss;
    PrettyPrintSizeof( oss << "epoll() on ", size ) << " fds, with the last one signalled.";

    Description desc;
    desc.measures = oss.str();

    return desc;
}


template<
    size_t num_fds
>
static autotime::BenchTimers MakeEpollTimers()
{
    auto p_fds = MakeEventFds( num_fds );
    auto p_epoll = std::make_shared< ScopedFile >(
        ScopedFile::make_bound( []()
            {
                int fd = epoll_create( num_fds );
                if (fd < 0) throw_system_error( errno, "epoll_create()" );
                return fd;
            }() ) );

    for (const ScopedFile &event: *p_fds)
    {
        epoll_data data{};
        data.fd = event.fd;
        constexpr uint32_t flags = EPOLLIN | EPOLLET;  // Benchmark also works witohut EPOLLET.
        epoll_event config = { flags, data };
        int rtn = epoll_ctl( p_epoll->fd, EPOLL_CTL_ADD, event.fd, &config );
        if (rtn < 0) throw_system_error( errno, "epoll_ctl()" );
    }

    std::function< Durations( int ) > timer = [p_fds, p_epoll]( int num_iter )
        {
            Durations durs{};

            for (int i = 0; i < num_iter; ++i)
            {
                // Run with one of the fds in the signalled state.
                uint64_t value = 1;
                Write( p_fds->back().fd, &value, sizeof( value ) );

                constexpr int max_events = 2;
                std::array< epoll_event, max_events > events{};

                // Perform a blocking poll with no timeout.
                TimePoints start_times = Start();
                constexpr int timeout_ms = -1;  // -1 blocks indefinitely.
                int result = epoll_wait( p_epoll->fd, events.data(), max_events, timeout_ms );
                durs += End( start_times );

                if (result < 0) throw_system_error( errno, "epoll_wait()" );
                assert( result == 1 );
                assert( events[0].events == EPOLLIN );
                assert( events[0].data.fd == p_fds->back().fd );

                // Reset the fd - needed for EPOLLET mode, but still impacts
                //  level-triggered mode, since the kernel is tracking it.
                Read( p_fds->back().fd, &value, sizeof( value ) );
            }

            return durs;
        };

    return { timer, MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::epoll_1 >()
{
    return DescribeEpoll< 1 << 0 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::epoll_1 >()
{
    return MakeEpollTimers< 1 << 0 >();
}


template<> Description Describe< Benchmark::epoll_8 >()
{
    return DescribeEpoll< 1 << 3 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::epoll_8 >()
{
    return MakeEpollTimers< 1 << 3 >();
}


template<> Description Describe< Benchmark::epoll_64 >()
{
    return DescribeEpoll< 1 << 6 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::epoll_64 >()
{
    return MakeEpollTimers< 1 << 6 >();
}


template<> Description Describe< Benchmark::epoll_768 >()
{
    return DescribeEpoll< 768 /* 1 << 9.5 */ >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::epoll_768 >()
{
    return MakeEpollTimers< 768 /* 1 << 9.5 */ >();
}


template<
    size_t size
>
static Description DescribePoll()
{
    std::ostringstream oss;
    PrettyPrintSizeof( oss << "poll() on ", size ) << " fds, with the last one signalled.";

    Description desc;
    desc.measures = oss.str();

    return desc;
}


template<
    size_t num_fds
>
static autotime::BenchTimers MakePollTimers()
{
    auto p_fds = MakeEventFds( num_fds );
    auto p_pollfds = std::make_shared< std::vector< pollfd > >( num_fds );
    for (size_t i = 0; i < p_fds->size(); ++i)
    {
        p_pollfds->at( i ) = { p_fds->at( i ).fd, POLLIN, 0 };
    }

    // Run with one of the fds in the signalled state.
    uint64_t value = 1;
    Write( p_fds->back().fd, &value, sizeof( value ) );

    std::function< void() > f = [p_fds, p_pollfds]()
        {
            // Perform a blocking poll with no timeout.
            constexpr int timeout_ms = -1;  // -1 blocks indefinitely.
            int result = poll( p_pollfds->data(), p_pollfds->size(), timeout_ms );

            if (result < 0) throw_system_error( errno, "poll()" );
            assert( result == 1 );
            assert( p_pollfds->back().revents == POLLIN );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::poll_1 >()
{
    return DescribePoll< 1 << 0 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::poll_1 >()
{
    return MakePollTimers< 1 << 0 >();
}


template<> Description Describe< Benchmark::poll_8 >()
{
    return DescribePoll< 1 << 3 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::poll_8 >()
{
    return MakePollTimers< 1 << 3 >();
}


template<> Description Describe< Benchmark::poll_64 >()
{
    return DescribePoll< 1 << 6 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::poll_64 >()
{
    return MakePollTimers< 1 << 6 >();
}


template<> Description Describe< Benchmark::poll_768 >()
{
    return DescribePoll< 768 /* 1 << 9.5 */ >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::poll_768 >()
{
    return MakePollTimers< 768 /* 1 << 9.5 */ >();
}


template<
    size_t size
>
static Description DescribeSelect()
{
    std::ostringstream oss;
    PrettyPrintSizeof( oss << "select() on ", size ) << " fds, with the last one signalled.";

    Description desc;
    desc.measures = oss.str();

    return desc;
}


template<
    size_t num_fds
>
static autotime::BenchTimers MakeSelectTimers()
{
    auto p_fds = MakeEventFds( num_fds );
    int max_fd = -1;
    auto p_fdset = std::make_shared< fd_set >();
    FD_ZERO( p_fdset.get() );
    for (const ScopedFile &event: *p_fds)
    {
        assert( event.fd < FD_SETSIZE );
        FD_SET( event.fd, p_fdset.get() );
        max_fd = std::max( max_fd, event.fd );
    }

    // Run with one of the fds in the signalled state.
    uint64_t value = 1;
    Write( p_fds->back().fd, &value, sizeof( value ) );

    std::function< Durations( int ) > timer = [p_fds, p_fdset, max_fd]( int num_iter )
        {
            Durations durs{};

            for (int i = 0; i < num_iter; ++i)
            {
                fd_set read_fds = *p_fdset;

                // Perform a blocking poll with no timeout.
                TimePoints start_times = Start();
                int result = select( max_fd + 1, &read_fds, nullptr, nullptr, nullptr );
                durs += End( start_times );

                if (result < 0) throw_system_error( errno, "select()" );
                assert( result == 1 );
                assert( FD_ISSET( p_fds->back().fd, &read_fds ) );
            }

            return durs;
        };

    return { timer, MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::select_1 >()
{
    return DescribeSelect< 1 << 0 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::select_1 >()
{
    return MakeSelectTimers< 1 << 0 >();
}


template<> Description Describe< Benchmark::select_8 >()
{
    return DescribeSelect< 1 << 3 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::select_8 >()
{
    return MakeSelectTimers< 1 << 3 >();
}


template<> Description Describe< Benchmark::select_64 >()
{
    return DescribeSelect< 1 << 6 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::select_64 >()
{
    return MakeSelectTimers< 1 << 6 >();
}


template<> Description Describe< Benchmark::select_768 >()
{
    return DescribeSelect< 768 /* 1 << 9.5 */ >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::select_768 >()
{
    return MakeSelectTimers< 768 /* 1 << 9.5 */ >();
}


} // namespace bench

