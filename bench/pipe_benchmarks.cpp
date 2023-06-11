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

#include <array>
#include <cassert>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include "autotime/os.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "error_utils.hpp"
#include "file_utils.hpp"
#include "thread_utils.hpp"
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


struct BlockingPipeTraits
{
    static void would_underflow()
    {
        throw std::runtime_error( "Pipe::read() underflow" );
    }

    static void would_overflow()
    {
        throw std::runtime_error( "Pipe::write() overflow" );
    }
};


struct NonblockingPipeTraits
{
    static inline void would_underflow()
    {
    }

    static inline void would_overflow()
    {
    }
};


template<
    typename traits_type=BlockingPipeTraits
>
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
        for (int fd: fds_) if (fd >= 0) ::close( fd );
    }

    Pipe &operator=( const Pipe & ) = delete;

    inline size_t read( uint8_t *p_dest, size_t n )
    {
        if (n > occupancy_) traits_type::would_underflow();

        size_t remain = n;
        while (remain > 0)
        {
            ssize_t result = ::read( fds_[0], p_dest + (n - remain), n );
            if (result < 0 && errno != EINTR) throw_system_error( errno, "Pipe::read()" );
            else if (result == 0) break;
            else
            {
                assert( result <= occupancy_ );
                assert( result <= remain );

                occupancy_ -= result;
                remain -= result;
            }
        }

        return n - remain;
    }

    inline void read_checked( uint8_t *p_dest, size_t n )
    {
        size_t got = this->read( p_dest, n );
        if (got < n) throw std::runtime_error( "Pipe::read() hit EOF" );
    }

    inline size_t write( const uint8_t *p_src, size_t n )
    {
        if (n + occupancy_ > capacity_) traits_type::would_underflow();

        size_t remain = n;
        while (remain > 0)
        {
            ssize_t result = ::write( fds_[1], p_src + (n - remain), n );
            if (result < 0 && errno != EINTR) throw_system_error( errno, "Pipe::write()" );
            else if (result == 0) break;
            else
            {
                assert( result <= remain );

                occupancy_ += result;
                remain -= result;
            }
        }

        return n - remain;
    }

    inline void write_checked( const uint8_t *p_dest, size_t n )
    {
        size_t got = this->write( p_dest, n );
        if (got < n) throw std::runtime_error( "Pipe::write() hit EOF" );
    }

    void drain()
    {
        buffer_.resize( occupancy_ );
        if (!buffer_.empty()) this->read_checked( buffer_.data(), buffer_.size() );
    }

    void fill()
    {
        size_t vacancy = capacity_ - occupancy_;
        if (vacancy < epsilon_) return;

        buffer_.resize( vacancy - 1);
        if (!buffer_.empty()) this->write_checked( buffer_.data(), buffer_.size() );
    }

        //! Closes the write side of the pipe, only.
    void close()
    {
        if (fds_[1] >= 0)
        {
            ::close( fds_[1] );
            fds_[1] = -1;
        }
    }
};


static Timer MakePipeOverheadTimer()
{
    std::shared_ptr< int > p = std::make_shared< int >();

    std::function< void() > o = [p]()
        {
            if (!p) throw std::runtime_error( "invalid pointer" );
        };

    return MakeTimer( o );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_read >()
{
    std::shared_ptr< Pipe<> > p_pipe = std::make_shared< Pipe<> >();

    Timer time_f = [p_pipe]( int num_iters )
        {
            p_pipe->fill();

            std::function< void() > f = [p_pipe]()
                {
                    uint8_t buf = 0;
                    p_pipe->read_checked( &buf, sizeof( buf ) );
                };

            return Time( f, num_iters );
        };

    return { time_f, MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_write >()
{
    std::shared_ptr< Pipe<> > p_pipe = std::make_shared< Pipe<> >();

    Timer time_f = [p_pipe]( int num_iters )
        {
            p_pipe->drain();

            std::function< void() > f = [p_pipe]()
                {
                    uint8_t buf = 0;
                    p_pipe->write_checked( &buf, sizeof( buf ) );
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
    std::shared_ptr< Pipe<> > p_pipe = std::make_shared< Pipe<> >();

    std::function< void() > f = [p_pipe]()
        {
            std::array< uint8_t, block_size > buf{};
            p_pipe->write_checked( buf.data(), buf.size() );
            p_pipe->read_checked( buf.data(), buf.size() );
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


struct Ponger
{
    const size_t message_size_;
    std::shared_ptr< Pipe< NonblockingPipeTraits >[] > pipes_;
    std::thread thread_;

    explicit Ponger( size_t message_size )
    :
        message_size_( message_size ),
        pipes_( new Pipe< NonblockingPipeTraits >[2] )
    {
        // Block on thread actually starting.
        std::promise< void > started_promise;
        std::future< void > started_future = started_promise.get_future(); 
        thread_ = std::thread( &Ponger::threadfunc, this, std::move( started_promise ) );
        started_future.get();
    }

    ~Ponger()
    {
        pipes_[0].close();  // A short read() will tell the thread to exit.
        thread_.join();
    }

    void threadfunc( std::promise< void > started_promise )
    {
        SetCoreAffinity( GetSecondaryCoreId() );

        started_promise.set_value();

        std::vector< uint8_t > buf( message_size_ );
        while (pipes_[0].read( buf.data(), buf.size() ) == message_size_)
        {
            pipes_[1].write_checked( buf.data(), buf.size() );
        }
    }
};


template<
    size_t block_size
>
static Timer MakePingPongTimer()
{
    Timer time_f = []( int num_iters )
        {
            Ponger ponger{ block_size };
            std::array< uint8_t, block_size > buf{};

            std::function< void() > f = [&ponger, &buf]()
                {
                    ponger.pipes_[0].write_checked( buf.data(), buf.size() );
                    ponger.pipes_[1].read_checked( buf.data(), buf.size() );
                };

            return Time( f, num_iters );
        };

    return time_f;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_pingpong_256 >()
{
    return { MakePingPongTimer< 1 << 8 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_pingpong_1k >()
{
    return { MakePingPongTimer< 1 << 10 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_pingpong_4k >()
{
    return { MakePingPongTimer< 1 << 12 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_pingpong_16k >()
{
    return { MakePingPongTimer< 1 << 14 >(), MakePipeOverheadTimer() };
}


template<> autotime::BenchTimers MakeTimers< Benchmark::pipe_pingpong_64k >()
{
    return { MakePingPongTimer< 1 << 16 >(), MakePipeOverheadTimer() };
}


} // namespace bench

