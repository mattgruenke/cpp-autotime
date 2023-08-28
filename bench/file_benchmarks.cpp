////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements benchmarks of basic file management and I/O operations.
/*! @file

    See Category::file, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>

#include "autotime/log.hpp"
#include "autotime/overhead.hpp"
#include "autotime/time.hpp"

#include "description.hpp"
#include "dispatch.hpp"
#include "error_utils.hpp"
#include "file_utils.hpp"
#include "list.hpp"


namespace filesystem = boost::filesystem;

using namespace autotime;

namespace bench
{


template<> Description Describe< Category::file >()
{
    Description desc;
    desc.measures = "Basic file management and I/O operations.";
    desc.notes = {
        "The device and filesystem measured are determined by the current working directory,"
        " when the benchmark is run.",
        "Beware that sync and datasysc operations will trigger host writes on the device"
        " for each iteration.",
        "Though randomized filenames are used for tests,"
        " conflicting files will generally result in failure.",
        "Use at your own risk." };
    return desc;
}


#if 0   // Not needed, since we can separately time create and unlink.

template<> Description Describe< Benchmark::file_create_unlink >()
{
    Description desc;
    desc.measures = "File creation and removal.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_create_unlink >()
{
    std::string filename = filesystem::unique_path().native();
    //if (std::ostream *debug = DebugLog()) *debug << "Using filename: " << filename;

    std::function< void() > f = [filename]()
        {
            ScopedFile file{ filename };
        };

    std::function< void() > o = [filename]()
        {
            const filesystem::path copy = filename;
        };

    return { MakeTimer( f ), MakeTimer( o ) };
}

#endif


template<> Description Describe< Benchmark::file_create >()
{
    Description desc;
    desc.measures = "File creation.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_create >()
{
    std::string filename = filesystem::unique_path().native();
    //if (std::ostream *debug = DebugLog()) *debug << "Using filename: " << filename;

    std::function< Durations( int ) > timer = [filename]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                TimePoints start_times = Start();
                Close( OpenFile( filename.c_str(), O_CREAT | O_RDWR ) );
                durs += End( start_times );

                Unlink( filename.c_str() );
            }

            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::file_unlink >()
{
    Description desc;
    desc.measures = "File removal.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_unlink >()
{
    std::string filename = filesystem::unique_path().native();

    std::function< Durations( int ) > timer = [filename]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                Close( OpenFile( filename.c_str(), O_CREAT | O_RDWR ) );

                TimePoints start_times = Start();
                Unlink( filename.c_str() );
                durs += End( start_times );
            }

            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::file_append >()
{
    Description desc;
    desc.measures = "Small writes to the end of a small file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_append >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();

    std::function< void() > f = [p_file]()
        {
            char c{};
            Write( p_file->fd, &c, 1 );
        };

    return { MakeTimer( f ), nullptr };
}


template<> Description Describe< Benchmark::file_close >()
{
    Description desc;
    desc.measures = "Closing a normal file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_close >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    p_file->close();

    std::function< Durations( int ) > timer = [p_file]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                int fd = OpenFile( p_file->filename.c_str(), O_RDWR );

                TimePoints start_times = Start();
                Close( fd );
                durs += End( start_times );
            }

            return durs;
        };

    return { timer, nullptr };
}


static autotime::BenchTimers MakeOpenTimers( int flags )
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    p_file->close();

    std::function< Durations( int ) > timer = [p_file, flags]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                TimePoints start_times = Start();
                int fd = OpenFile( p_file->filename.c_str(), flags );
                durs += End( start_times );

                Close( fd );
            }

            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::file_open_ro >()
{
    Description desc;
    desc.measures = "Opening an existing file as read-only.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_open_ro >()
{
    return MakeOpenTimers( O_RDONLY );
}


template<> Description Describe< Benchmark::file_open_rw >()
{
    Description desc;
    desc.measures = "Opening an existing file as read/write.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_open_rw >()
{
    return MakeOpenTimers( O_RDWR );
}


template<> Description Describe< Benchmark::file_open_trunc >()
{
    Description desc;
    desc.measures = "Opening an existing file as read/write, with truncation.";
    desc.notes = { "The file being used is already empty." };
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_open_trunc >()
{
    return MakeOpenTimers( O_RDWR | O_TRUNC );
}


template<> Description Describe< Benchmark::file_open_nonexistent >()
{
    Description desc;
    desc.measures = "Opening a nonexistent file as read/write.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_open_nonexistent >()
{
    std::string filename = filesystem::unique_path().native();

    std::function< void() > f = [filename]()
        {
            constexpr mode_t mode = 0;
            int fd = open( filename.c_str(), O_RDWR, mode );
            if (fd >= 0) throw std::runtime_error( "open() unexpectedly succeeded." );
            else if (errno != ENOENT) throw_system_error( errno, "open()" );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::file_stat >()
{
    Description desc;
    desc.measures = "stat() on a normal file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_stat >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    p_file->close();

    std::function< void() > f = [p_file]()
        {
            struct stat s{};
            int rtn = stat( p_file->filename.c_str(), &s );
            if (rtn < 0) throw_system_error( errno, "stat()" );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::file_stat_nonexistent >()
{
    Description desc;
    desc.measures = "stat() on a nonexistent file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_stat_nonexistent >()
{
    std::string filename = filesystem::unique_path().native();

    std::function< void() > f = [filename]()
        {
            struct stat s{};
            int rtn = stat( filename.c_str(), &s );
            if (rtn >= 0) throw std::runtime_error( "stat() unexpectedly succeeded." );
            else if (errno != ENOENT) throw_system_error( errno, "stat()" );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


static blksize_t GetBlockSize( int fd )
{
    struct stat s{};
    int rtn = fstat( fd, &s );
    if (rtn < 0) throw_system_error( errno, "fstat()" );

    return s.st_blksize;
}

 
static autotime::BenchTimers MakeFsyncTimers( std::shared_ptr< ScopedFile > p_file, size_t len )
{
    std::vector< uint8_t > buf;
    buf.resize( len );

    std::function< Durations( int ) > timer = [p_file, buf]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                if (!buf.empty())
                {
                    int rtn = lseek( p_file->fd, 0, SEEK_SET );
                    if (rtn < 0) throw_system_error( errno, "lseek()" );

                    Write( p_file->fd, buf.data(), buf.size() );
                }

                TimePoints start_times = Start();
                int rtn = fsync( p_file->fd );
                durs += End( start_times );

                if (rtn < 0) throw_system_error( errno, "fsync()" );
            }

            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::file_fsync_0 >()
{
    Description desc;
    desc.measures = "fsync() on a normal file, with 0 bytes written.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_fsync_0 >()
{
#if 0
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();

    std::function< void() > f = [p_file]()
        {
            int rtn = fsync( p_file->fd );
            if (rtn < 0) throw_system_error( errno, "fsync()" );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
#else
    return MakeFsyncTimers( std::make_shared< ScopedFile >(), 0 );
#endif
}


template<> Description Describe< Benchmark::file_fsync_1 >()
{
    Description desc;
    desc.measures = "fsync() on a normal file, with 1 bytes written.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_fsync_1 >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    FillFile( p_file->fd, GetBlockSize( p_file->fd ) );

    return MakeFsyncTimers( p_file, 1 );
}


template<> Description Describe< Benchmark::file_fsync_block >()
{
    Description desc;
    desc.measures = "fsync() on a normal file, with 1 bytes written.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_fsync_block >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    size_t blk_size = GetBlockSize( p_file->fd );
    FillFile( p_file->fd, blk_size );

    return MakeFsyncTimers( p_file, blk_size );
}


} // namespace bench

