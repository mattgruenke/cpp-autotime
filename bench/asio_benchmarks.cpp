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
#include <boost/optional.hpp>

#include "autotime/os.hpp"
#include "autotime/overhead.hpp"
#include "autotime/overhead_impl.hpp"
#include "autotime/time.hpp"

#include "description.hpp"
#include "dispatch.hpp"
#include "error_utils.hpp"
#include "list.hpp"
#include "pipe_utils.hpp"
#include "thread_utils.hpp"


namespace asio = boost::asio;

using namespace autotime;

namespace bench
{


template<> Description Describe< Category::asio >()
{
    Description desc;
    desc.measures = "Various Boost.ASIO io_service functions and communication mechanisms.";
    return desc;
}


struct AsioCounter: std::enable_shared_from_this< AsioCounter >
{
    asio::io_service iosvc_;
    boost::optional< asio::io_service::strand > strand_;
    int i_ = 0;
    std::function< void() > cb_;

    AsioCounter()
    {
    }

    std::thread StartIoThread()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();
        auto threadfunc = [p_this]()
            {
                SetCoreAffinity( GetSecondaryCoreId() );
                p_this->iosvc_.run();
            };

        std::promise< void > thread_started;
        p_this->iosvc_.post( [&thread_started]() { thread_started.set_value(); } );
        std::thread thread = std::thread{ threadfunc };
        thread_started.get_future().wait();

        return thread;
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

    Timer MakeGetIoServiceTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        std::function< void() > o = [p_this]() -> const asio::io_service &
            {
                return p_this->iosvc_;
            };

        return MakeTimer( o );
    }

    Timer MakeIoResetTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        std::function< void() > f = [p_this]()
            {
                p_this->iosvc_.reset();
            };

        return MakeTimer( f );
    }

    Timer MakeIoRunTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        std::function< void() > f = [p_this]()
            {
                p_this->iosvc_.reset();
                p_this->iosvc_.run();
            };

        return MakeTimer( f );
    }

    Timer MakeDispatchTimer( bool separate_thread, bool wait, bool use_strand )
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        if (use_strand) strand_.emplace( iosvc_ );

        return [p_this, separate_thread, wait, use_strand]( int num_iters )
            {
                p_this->iosvc_.reset();

                boost::optional< asio::io_service::work > work;
                std::thread thread;
                if (separate_thread)
                {
                    work.emplace( p_this->iosvc_ );
                    thread = p_this->StartIoThread();
                }

                TimePoints start_times = Start();

                for (int i = 0; i < num_iters-1; ++i)
                {
                    if (use_strand) p_this->strand_->dispatch( [](){} );
                    else p_this->iosvc_.dispatch( [](){} );
                }

                std::promise< void > thread_sync;
                auto sync_cb = [&thread_sync]() { thread_sync.set_value(); };
                if (use_strand) p_this->strand_->dispatch( sync_cb );
                else p_this->iosvc_.dispatch( sync_cb );

                Durations durs{};
                if (!wait) durs = End( start_times );
                else if (separate_thread)
                {
                    thread_sync.get_future().wait();
                    durs = End( start_times );
                }
                else
                {
                    p_this->iosvc_.run();
                    durs = End( start_times );
                }

                if (separate_thread)
                {
                    work.reset();
                    thread.join();
                }

                return End( start_times );
            };
    }

    Timer MakeTailPostTimer( bool use_strand )
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        if (use_strand) strand_.emplace( iosvc_ );

        return [p_this, use_strand]( int num_iters )
            {
                p_this->iosvc_.reset();
                p_this->i_ = 0;

                AsioCounter *p = p_this.get();
                p_this->cb_ = [p, num_iters, use_strand]()
                    {
                        if (p->i_++ >= num_iters) return;

                        if (use_strand) p->strand_->post( p->cb_ );
                        else p->iosvc_.post( p->cb_ );
                    };

                TimePoints start_times = Start();

                if (use_strand) p_this->strand_->post( p_this->cb_ );
                else p_this->iosvc_.post( p_this->cb_ );

                p_this->iosvc_.run();

                return End( start_times );
            };
    }

    Timer MakePostLoopTimer( bool separate_thread, bool wait )
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();

        return [p_this, separate_thread, wait]( int num_iters )
            {
                p_this->iosvc_.reset();

                boost::optional< asio::io_service::work > work;
                std::thread thread;
                if (separate_thread)
                {
                    work.emplace( p_this->iosvc_ );
                    thread = p_this->StartIoThread();
                }

                TimePoints start_times = Start();

                for (int i = 0; i < num_iters - 1; ++i) p_this->iosvc_.post( [](){} );

                std::promise< void > thread_sync;
                p_this->iosvc_.post( [&thread_sync]() { thread_sync.set_value(); } );

                Durations durs{};
                if (!wait) durs = End( start_times );
                else if (separate_thread)
                {
                    thread_sync.get_future().wait();
                    durs = End( start_times );
                }
                else
                {
                    p_this->iosvc_.run();
                    durs = End( start_times );
                }

                if (separate_thread)
                {
                    work.reset();
                    thread.join();
                }

                return durs;
            };
    }

    Timer MakePingPongTimer()
    {
        std::shared_ptr< AsioCounter > p_this = shared_from_this();
        std::shared_ptr< AsioCounter > p_other = std::make_shared< AsioCounter >();

        return [p_this, p_other]( int num_iters )
            {
                p_this->iosvc_.reset();
                p_other->iosvc_.reset();
                p_other->i_ = 0;

                // Keep io_service::run() from returning until we're done.
                std::vector< asio::io_service::work > work;
                work.emplace_back( p_this->iosvc_ );
                work.emplace_back( p_other->iosvc_ );

                std::thread thread = p_other->StartIoThread();

                AsioCounter *p2 = p_other.get();
                p_this->cb_ = [p2, num_iters, &work]()
                    {
                        if (p2->i_++ < num_iters) p2->iosvc_.post( p2->cb_ );
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

                thread.join();

                return durs;
            };
    }
};


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer( false, false, false ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch_wait >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer( false, true, false ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch_strand >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer( false, false, true ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch_strand_wait >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer( false, true, true ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch_threaded >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer( true, false, false ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch_threaded_wait >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer( true, true, false ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_dispatch_strand_threaded_wait >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeDispatchTimer( true, true, true ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakePostLoopTimer( false, false ), nullptr };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_wait >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakePostLoopTimer( false, true ), nullptr };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_threaded >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakePostLoopTimer( true, false ), nullptr };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_threaded_wait >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakePostLoopTimer( true, true ), nullptr };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_tail >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeTailPostTimer( false ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_tail_strand >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeTailPostTimer( true ), counter->MakeOverheadTimer() };
}


template<> BenchTimers MakeTimers< Benchmark::asio_post_pingpong >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakePingPongTimer(), counter->MakeOverheadTimer() };
}


template<> Description Describe< Benchmark::asio_reset >()
{
    Description desc;
    desc.measures = "asio::io_service::reset() on an empty io_service.";
    desc.overhead = "Accessing the io_service via a shared_ptr, from within a lambda.";
    return desc;
}


template<> BenchTimers MakeTimers< Benchmark::asio_reset >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeIoResetTimer(), counter->MakeGetIoServiceTimer() };
}


template<> Description Describe< Benchmark::asio_run >()
{
    Description desc;
    desc.measures = "asio::io_service::run() on an empty io_service.";
    desc.overhead = "First calling asio::io_service::reset().";
    return desc;
}


template<> BenchTimers MakeTimers< Benchmark::asio_run >()
{
    std::shared_ptr< AsioCounter > counter = std::make_shared< AsioCounter >();
    return { counter->MakeIoRunTimer(), counter->MakeIoResetTimer() };
}


struct Pipe
{
    const size_t capacity_;
    std::vector< asio::posix::stream_descriptor > fds_;

    explicit Pipe( asio::io_service &iosvc )
    :
        Pipe( iosvc, iosvc )
    {
    }

    Pipe( asio::io_service &iosvc_rd, asio::io_service &iosvc_wr )
    :
        capacity_{ GetMaxPipeSize() }
    {
        int fds[2] = { -1, -1 };
        OpenPipe( fds );

        // Transfer ownership of the fds to asio, which handles closing them.
        fds_.emplace_back( iosvc_rd, fds[0] );
        fds_.emplace_back( iosvc_wr, fds[1] );

        SetPipeSize( fds, capacity_ );
    }
};


template<
    typename descriptor_type
>
struct Stream
{
    descriptor_type &desc_;
    int num_iters_ = 0;
    int i_ = 0;
    const size_t io_size = 1;
    std::vector< uint8_t > storage_;
    asio::mutable_buffers_1 buffer_;
    std::promise< void > done_promise_;
    using cb_type = std::function< void( const boost::system::error_code &, size_t ) >;
    using op_type = void (descriptor_type::*)( const asio::mutable_buffers_1 &, cb_type && );
    op_type op_ = nullptr;

    Stream( descriptor_type &desc )
    :
        desc_{ desc },
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
        else if (i_++ >= num_iters_)
        {
            done_promise_.set_value();
            return;
        }

        (desc_.*op_)( buffer_, std::bind( &Stream::iterate, this, std::placeholders::_1 ) );
    }

    void repeat_async( op_type op, int num_iters )
    {
        // Setup class members used by iterate().
        num_iters_ = num_iters;
        i_ = 0;
        op_ = op;

        // Initiate first iteration.
        (desc_.*op)( buffer_, std::bind( &Stream::iterate, this, std::placeholders::_1 ) );
    }

    void read_async( int num_iters )
    {
        this->repeat_async(
            &descriptor_type::template async_read_some< asio::mutable_buffers_1, cb_type >,
            num_iters );
    }

    void write_async( int num_iters )
    {
        this->repeat_async(
            &descriptor_type::template async_write_some< asio::mutable_buffers_1, cb_type >,
            num_iters );
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
    typename descriptor_type
>
struct Ponger
{
    using stream_type = Stream< descriptor_type >;

    const int num_iters_;
    stream_type &read_stream_;
    stream_type &write_stream_;

    Ponger( int num_iters, stream_type &read_stream, stream_type &write_stream )
    :
        num_iters_{ num_iters },
        read_stream_{ read_stream },
        write_stream_{ write_stream }
    {
    }

    void throw_if( const boost::system::error_code &error )
    {
        if (error)
        {
            if (error.value() != EINTR) throw boost::system::system_error( error );
        }
    }

    void async_read()
    {
        if (read_stream_.i_++ >= num_iters_) return;

        using namespace std::placeholders;
        read_stream_.desc_.async_read_some( read_stream_.buffer_,
            std::bind( &Ponger::read_handler, this, _1, _2 ) );
    }

    void async_write()
    {
        if (write_stream_.i_++ >= num_iters_) return;

        using namespace std::placeholders;
        write_stream_.desc_.async_write_some( write_stream_.buffer_, 
            std::bind( &Ponger::write_handler, this, _1, _2 ) );
    }

    void read_handler( const boost::system::error_code &error, size_t bytes_transferred )
    {
        this->throw_if( error );
        if (bytes_transferred != read_stream_.storage_.size())
        {
            throw std::runtime_error( "Incomplete read" );
        }
        this->async_write();
    }

    void write_handler( const boost::system::error_code &error, size_t bytes_transferred )
    {
        this->throw_if( error );
        if (bytes_transferred != write_stream_.storage_.size())
        {
            throw std::runtime_error( "Incomplete write" );
        }
        this->async_read();
    }
};


static BenchTimers MakePingPongTimer( bool second_thread )
{
    Timer time_f = [second_thread]( int num_iters )
        {
            asio::io_service iosvc0;
            asio::io_service iosvc1;

            Pipe pipe0{ iosvc0, second_thread ? iosvc1 : iosvc0 };
            Pipe pipe1{ second_thread ? iosvc1 : iosvc0, iosvc0 };
            using descriptor_type = asio::posix::stream_descriptor;
            Stream< descriptor_type > pipe0_rd{ pipe0.fds_[0] };
            Stream< descriptor_type > pipe0_wr{ pipe0.fds_[1] };
            Stream< descriptor_type > pipe1_rd{ pipe1.fds_[0] };
            Stream< descriptor_type > pipe1_wr{ pipe1.fds_[1] };
            Ponger< descriptor_type > ponger0{ num_iters, pipe0_rd, pipe1_wr };
            Ponger< descriptor_type > ponger1{ num_iters, pipe1_rd, pipe0_wr };

            ponger1.async_read();

            std::thread thread;
            if (second_thread)
            {
                auto threadfunc = [&iosvc1]()
                    {
                        SetCoreAffinity( GetSecondaryCoreId() );
                        iosvc1.run();
                    };

                std::promise< void > thread_started;
                iosvc1.post( [&thread_started]() { thread_started.set_value(); } );
                thread = std::thread{ threadfunc };
                thread_started.get_future().wait();
            }

            TimePoints start_times = Start();
            ponger0.async_write();
            iosvc0.run();
            Durations durs = End( start_times );

            if (second_thread) thread.join();

            return durs;
        };

    return { time_f, nullptr };
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_pingpong >()
{
    return MakePingPongTimer( false );
}


template<> BenchTimers MakeTimers< Benchmark::pipe_asio_pingpong_threaded >()
{
    return MakePingPongTimer( true );
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

