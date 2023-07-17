////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Uses Boost.ASIO to implement benchmarks in categories: pipe and socket.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <future>
#include <memory>
#include <system_error>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

#include "autotime/os.hpp"
#include "autotime/overhead.hpp"
#include "autotime/overhead_impl.hpp"
#include "autotime/time.hpp"

#include "dispatch.hpp"
#include "error_utils.hpp"
#include "list.hpp"
#include "pipe_utils.hpp"
#include "thread_utils.hpp"


namespace asio = boost::asio;

using namespace autotime;

namespace bench
{


struct AsioCounter: std::enable_shared_from_this< AsioCounter >
{
    asio::io_service iosvc_;
    asio::io_service::strand strand_;
    int i_ = 0;
    std::function< void() > cb_;

    AsioCounter()
    :
        strand_{ iosvc_ }
    {
    }

    static Timer MakeOverheadTimer()
    {
        std::function< void() > nullary;
        std::function< void() > o = [nullary]()
            {
                Overhead_void< std::function< void() > >( nullary );
            };

        return MakeTimer( o );
    }

    Timer MakeDispatchTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        return [p_this]( int num_iters )
            {
                p_this->iosvc_.reset();

                TimePoints start_times = Start();

                for (int i = 0; i < num_iters; ++i) p_this->iosvc_.dispatch( [](){} );
                p_this->iosvc_.run();

                return End( start_times );
            };
    }

    Timer MakeStrandDispatchTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        return [p_this]( int num_iters )
            {
                p_this->iosvc_.reset();

                TimePoints start_times = Start();

                for (int i = 0; i < num_iters; ++i) p_this->strand_.dispatch( [](){} );
                p_this->iosvc_.run();

                return End( start_times );
            };
    }

    Timer MakePostTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        return [p_this]( int num_iters )
            {
                p_this->iosvc_.reset();
                p_this->i_ = 0;

                AsioCounter *p = p_this.get();
                p_this->cb_ = [p, num_iters]()
                    {
                        if (++p->i_ < num_iters) p->iosvc_.post( p->cb_ );
                    };

                TimePoints start_times = Start();

                p_this->iosvc_.post( p_this->cb_ );
                p_this->iosvc_.run();

                return End( start_times );
            };
    }

    Timer MakeStrandPostTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        return [p_this]( int num_iters )
            {
                p_this->iosvc_.reset();
                p_this->i_ = 0;

                AsioCounter *p = p_this.get();
                p_this->cb_ = [p, num_iters]()
                    {
                        if (++p->i_ < num_iters) p->strand_.post( p->cb_ );
                    };

                TimePoints start_times = Start();

                p_this->strand_.post( p_this->cb_ );
                p_this->iosvc_.run();

                return End( start_times );
            };
    }

    Timer MakePingPongTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();
        std::shared_ptr< AsioCounter > p_other = std::make_shared< AsioCounter >();
        std::shared_ptr< std::thread > p_thread = std::make_shared< std::thread >();

        return [p_this, p_other, p_thread]( int num_iters )
            {
                p_this->iosvc_.reset();
                p_other->iosvc_.reset();
                p_other->i_ = 0;

                // Keep io_service::run() from returning until we're done.
                std::vector< asio::io_service::work > work;
                work.emplace_back( p_this->iosvc_ );
                work.emplace_back( p_other->iosvc_ );

                auto threadfunc = [p_other]()
                    {
                        SetCoreAffinity( GetSecondaryCoreId() );
                        p_other->iosvc_.run();
                    };

                *p_thread = std::thread( threadfunc );

                AsioCounter *p2 = p_other.get();
                p_this->cb_ = [p2, num_iters, &work]()
                    {
                        if (++p2->i_ < num_iters) p2->iosvc_.post( p2->cb_ );
                        else work.clear();
                    };

                AsioCounter *p = p_this.get();
                p_other->cb_ = [p]()
                    {
                        p->iosvc_.post( p->cb_ );
                    };

                TimePoints start_times = Start();
                p_other->iosvc_.post( p2->cb_ );
                p_this->iosvc_.run();
                Durations durs = End( start_times );

                p_thread->join();

                return durs;
            };
    }
};


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer(), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch_strand >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeStrandDispatchTimer(), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakePostTimer(), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_strand >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeStrandPostTimer(), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_pingpong >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakePingPongTimer(), counter->MakeOverheadTimer() };
}


struct Pipe
{
    const size_t capacity_;
    std::vector< asio::posix::stream_descriptor > fds_;

    explicit Pipe( asio::io_service &iosvc )
    :
        capacity_{ GetMaxPipeSize() }
    {
        int fds[2] = { -1, -1 };
        OpenPipe( fds );

        // Transfer ownership of the fds to asio, which handles closing them.
        for (int fd: fds) fds_.emplace_back( iosvc, fd );

        SetPipeSize( fds, capacity_ );
    }
};


template<
    typename descriptor_type
>
struct Stream
{
    descriptor_type &stream_;
    size_t num_iter_ = 0;
    size_t i_ = 0;
    const size_t io_size = 1;
    std::vector< uint8_t > storage_;
    asio::mutable_buffers_1 buffer_;
    std::promise< void > done_promise_;
    using cb_type = std::function< void( const boost::system::error_code &, size_t ) >;
    using op_type = void (descriptor_type::*)( const asio::mutable_buffers_1 &, cb_type && );
    op_type op_ = nullptr;

    Stream( descriptor_type &stream )
    :
        stream_{ stream },
        storage_( io_size ),
        buffer_( asio::buffer( storage_ ) )
    {
    }

    void iterate( const boost::system::error_code &error )
    {
        if (error)
        {
            if (error.value() != EINTR) throw boost::system::system_error( error );
        }
        else if (++i_ >= num_iter_)
        {
            done_promise_.set_value();
            return;
        }

        (stream_.*op_)( buffer_, std::bind( &Stream::iterate, this, std::placeholders::_1 ) );
    }

    void repeat_async( op_type op, size_t num_iter )
    {
        // Setup class members used by iterate().
        num_iter_ = num_iter;
        i_ = 0;
        op_ = op;

        // Initiate first iteration.
        (stream_.*op)( buffer_, std::bind( &Stream::iterate, this, std::placeholders::_1 ) );
    }

    void read_async( size_t num_iter )
    {
        this->repeat_async(
            &descriptor_type::template async_read_some< asio::mutable_buffers_1, cb_type >,
            num_iter );
    }

    void write_async( size_t num_iter )
    {
        this->repeat_async(
            &descriptor_type::template async_write_some< asio::mutable_buffers_1, cb_type >,
            num_iter );
    }

    std::future< void > getFuture()
    {
        // Setup the promise & future used for synchronization.
        done_promise_ = std::promise< void >{};
        return done_promise_.get_future();
    }
};


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_read >()
{
    std::shared_ptr< asio::io_service > p_iosvc = std::make_shared< asio::io_service >();

    Timer time_f = [p_iosvc]( int num_iters )
        {
            // Create the raw pipe and fill to max capacity.
            Pipe pipe{ *p_iosvc };
            std::vector< uint8_t > data( pipe.capacity_ );
            asio::write( pipe.fds_[1], asio::buffer( data ) );

            data.resize( 1 );
            auto buffer = asio::buffer( data );
            std::function< void() > f = [&pipe, &buffer]()
                {
                    asio::read( pipe.fds_[0], buffer );
                };

            return Time( f, num_iters );
        };

    std::function< void() > o = []()
        {
            int i = 1;
            int j = 2;
            Overhead_void< int &, int & >( i, j );
        };

    return { time_f, MakeTimer( o ) };
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_read_async >()
{
    std::shared_ptr< asio::io_service > p_iosvc = std::make_shared< asio::io_service >();

    Timer time_f = [p_iosvc]( int num_iters )
        {
            p_iosvc->reset();

            // Create the raw pipe and fill to max capacity.
            Pipe pipe{ *p_iosvc };
            std::vector< uint8_t > data( pipe.capacity_ );
            asio::write( pipe.fds_[1], asio::buffer( data ) );

            // Create the stream and run().
            Stream< asio::posix::stream_descriptor > stream{ pipe.fds_[0] };

            // Instead of using Time() to iterate, Stream::read_async() does it internally.
            TimePoints start_times = Start();

            stream.read_async( num_iters );
            p_iosvc->run();

            return End( start_times );
        };

    return { time_f, nullptr };
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_write >()
{
    std::shared_ptr< asio::io_service > p_iosvc = std::make_shared< asio::io_service >();

    Timer time_f = [p_iosvc]( int num_iters )
        {
            Pipe pipe{ *p_iosvc };
            std::vector< uint8_t > data( 1 );
            auto buffer = asio::buffer( data );
            std::function< void() > f = [&pipe, &buffer]()
                {
                    asio::write( pipe.fds_[1], buffer );
                };

            return Time( f, num_iters );
        };

    std::function< void() > o = []()
        {
            int i = 1;
            int j = 2;
            Overhead_void< int &, int & >( i, j );
        };

    return { time_f, MakeTimer( o ) };
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_write_async >()
{
    std::shared_ptr< asio::io_service > p_iosvc = std::make_shared< asio::io_service >();

    Timer time_f = [p_iosvc]( int num_iters )
        {
            p_iosvc->reset();

            Pipe pipe{ *p_iosvc };
            Stream< asio::posix::stream_descriptor > stream{ pipe.fds_[1] };

            // Instead of using Time() to iterate, Stream::read_async() does it internally.
            TimePoints start_times = Start();

            stream.write_async( num_iters );
            p_iosvc->run();

            return End( start_times );
        };

    return { time_f, nullptr };
}


template<
    size_t block_size
>
static BenchTimers MakeWriteReadTimer()
{
    std::shared_ptr< asio::io_service > p_iosvc = std::make_shared< asio::io_service >();

    Timer time_f = [p_iosvc]( int num_iters )
        {
            Pipe pipe{ *p_iosvc };
            std::array< uint8_t, block_size > data{};
            auto buffer = asio::buffer( data );
            std::function< void() > f = [&pipe, &buffer]()
                {
                    asio::write( pipe.fds_[1], buffer );
                    asio::read( pipe.fds_[0], buffer );
                };

            return Time( f, num_iters );
        };

    std::function< void() > o = []()
        {
            int i = 1;
            int j = 2;
            Overhead_void< int &, int & >( i, j );
            Overhead_void< int &, int & >( i, j );
        };

    return { time_f, MakeTimer( o ) };
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_write_read_256 >()
{
    return MakeWriteReadTimer< 1 << 8 >();
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_write_read_1k >()
{
    return MakeWriteReadTimer< 1 << 10 >();
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_write_read_4k >()
{
    return MakeWriteReadTimer< 1 << 12 >();
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_write_read_16k >()
{
    return MakeWriteReadTimer< 1 << 14 >();
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_write_read_64k >()
{
    return MakeWriteReadTimer< 1 << 16 >();
}


} // namespace bench

