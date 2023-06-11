////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements non-Boost.ASIO pipe benchmarks.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "error_utils.hpp"
#include "file_utils.hpp"
#include "list.hpp"


using namespace autotime;

namespace bench
{


static size_t GetMaxPipeSize()
{
    // Cache the value for subsequent calls.
    static const int result = ReadFromFile< int >( "/proc/sys/fs/pipe-max-size" );

    if (result <= 0) throw std::runtime_error(
            "Invalid pipe max size: " + std::to_string( result ) );

    return static_cast< size_t >( result );
}


static void SetPipeSize( int fds[2], int size )
{
    int actual = fcntl( fds[1], F_SETPIPE_SZ, size );
    if (actual < 0) throw_system_error( errno, "Set pipe size" );
    else if (actual < size) throw std::runtime_error(
        "Set pipe size failed: " + std::to_string( actual ) + " < " + std::to_string( size ) );
}


struct Pipe
{
    const size_t capacity_;
    static constexpr size_t epsilon_ = 4096;   // Minimum amount to pre-fill or post-drain pipe.
    size_t occupancy_ = 0;
    int fds_[2] = { -1, -1 };
    std::vector< uint8_t > buffer_;

    Pipe()
    :
        capacity_{ GetMaxPipeSize() }
    {
        if (pipe( fds_ )) throw_system_error( errno, "pipe()" );

        SetPipeSize( fds_, capacity_ );
    }

    Pipe( const Pipe & ) = delete;

    ~Pipe()
    {
        for (int fd: fds_) if (fd >= 0) close( fd );
    }

    Pipe &operator=( const Pipe & ) = delete;

    inline void read( uint8_t *p_dest, size_t n )
    {
        if (n > occupancy_) throw std::runtime_error( "Pipe::read() underflow" );

        size_t remain = n;
        while (remain > 0)
        {
            ssize_t result = ::read( fds_[0], p_dest + (n - remain), n );
            if (result < 0 && errno != EINTR) throw_system_error( errno, "Pipe::read()" );
            else if (result == 0) throw std::runtime_error( "Pipe::read() hit EOF" );
            else
            {
                assert( result <= occupancy_ );
                assert( result <= remain );

                occupancy_ -= result;
                remain -= result;
            }
        }
    }

    inline void write( const uint8_t *p_src, size_t n )
    {
        if (n + occupancy_ > capacity_) throw std::runtime_error( "Pipe::write() overflow" );

        size_t remain = n;
        while (remain > 0)
        {
            ssize_t result = ::write( fds_[1], p_src + (n - remain), n );
            if (result < 0 && errno != EINTR) throw_system_error( errno, "Pipe::write()" );
            else if (result == 0) throw std::runtime_error( "Pipe::write() hit EOF" );
            else
            {
                assert( result <= remain );

                occupancy_ += result;
                remain -= result;
            }
        }
    }

    void drain()
    {
        buffer_.resize( occupancy_ );
        if (!buffer_.empty()) this->read( buffer_.data(), buffer_.size() );
    }

    void fill()
    {
        size_t vacancy = capacity_ - occupancy_;
        if (vacancy < epsilon_) return;

        buffer_.resize( vacancy - 1);
        if (!buffer_.empty()) this->write( buffer_.data(), buffer_.size() );
    }
};


static Timer MakePipeOverheadTimer()
{
    std::shared_ptr< Pipe > p_pipe = std::make_shared< Pipe >();

    std::function< void() > o = [p_pipe]()
        {
            if (!p_pipe) throw std::runtime_error( "invalid pipe" );
        };

    return MakeTimer( o );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_read >()
{
    std::shared_ptr< Pipe > p_pipe = std::make_shared< Pipe >();

    Timer time_f = [p_pipe]( int num_iters )
        {
            p_pipe->fill();

            std::function< void() > f = [p_pipe]()
                {
                    uint8_t buf = 0;
                    p_pipe->read( &buf, sizeof( buf ) );
                };

            return Time( f, num_iters );
        };

    return { time_f, MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_write >()
{
    std::shared_ptr< Pipe > p_pipe = std::make_shared< Pipe >();

    Timer time_f = [p_pipe]( int num_iters )
        {
            p_pipe->drain();

            std::function< void() > f = [p_pipe]()
                {
                    uint8_t buf = 0;
                    p_pipe->write( &buf, sizeof( buf ) );
                };

            return Time( f, num_iters );
        };

    return { time_f, MakePipeOverheadTimer() };
}


template<
    size_t block_size
>
static Timer MakeWriteReadTimer()
{
    std::shared_ptr< Pipe > p_pipe = std::make_shared< Pipe >();

    std::function< void() > f = [p_pipe]()
        {
            std::array< uint8_t, block_size > buf{};
            p_pipe->write( buf.data(), buf.size() );
            p_pipe->read( buf.data(), buf.size() );
        };

    return MakeTimer( f );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_write_read_256 >()
{
    return { MakeWriteReadTimer< 1 << 8 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_write_read_1k >()
{
    return { MakeWriteReadTimer< 1 << 10 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_write_read_4k >()
{
    return { MakeWriteReadTimer< 1 << 12 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_write_read_16k >()
{
    return { MakeWriteReadTimer< 1 << 14 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_write_read_64k >()
{
    return { MakeWriteReadTimer< 1 << 16 >(), MakePipeOverheadTimer() };
}


} // namespace bench

