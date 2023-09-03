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
#include "format_utils.hpp"


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

 
static autotime::BenchTimers MakeFsyncTimers(
    std::shared_ptr< ScopedFile > p_file, size_t len, int (*f)( int ) )
{
    std::vector< uint8_t > buf;
    buf.resize( len );

    std::function< Durations( int ) > timer = [p_file, f, buf]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                if (!buf.empty())
                {
                    LSeek( p_file->fd, 0, SEEK_SET );
                    Write( p_file->fd, buf.data(), buf.size() );
                }

                TimePoints start_times = Start();
                int rtn = f( p_file->fd );
                durs += End( start_times );

                if (rtn < 0) throw_system_error( errno, "fsync()/fdatasync()" );
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
    return MakeFsyncTimers( std::make_shared< ScopedFile >(), 0, fsync );
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

    return MakeFsyncTimers( p_file, 1, fsync );
}


template<> Description Describe< Benchmark::file_fsync_block >()
{
    Description desc;
    desc.measures = "fsync() on a normal file, with a complete filesystem block written.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_fsync_block >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    size_t blk_size = GetBlockSize( p_file->fd );
    FillFile( p_file->fd, blk_size );

    return MakeFsyncTimers( p_file, blk_size, fsync );
}


template<> Description Describe< Benchmark::file_fdatasync_0 >()
{
    Description desc;
    desc.measures = "fdatasync() on a normal file, with 0 bytes written.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_fdatasync_0 >()
{
    return MakeFsyncTimers( std::make_shared< ScopedFile >(), 0, fdatasync );
}


template<> Description Describe< Benchmark::file_fdatasync_1 >()
{
    Description desc;
    desc.measures = "fdatasync() on a normal file, with 1 bytes written.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_fdatasync_1 >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    FillFile( p_file->fd, GetBlockSize( p_file->fd ) );

    return MakeFsyncTimers( p_file, 1, fdatasync );
}


template<> Description Describe< Benchmark::file_fdatasync_block >()
{
    Description desc;
    desc.measures = "fdatasync() on a normal file, with a complete filesystem block written.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_fdatasync_block >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    size_t blk_size = GetBlockSize( p_file->fd );
    FillFile( p_file->fd, blk_size );

    return MakeFsyncTimers( p_file, blk_size, fdatasync );
}


template<> Description Describe< Benchmark::file_lseek_random >()
{
    Description desc;
    desc.measures = "lseek() to a random position within a 1 MiB file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_lseek_random >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    constexpr size_t size = 1 << 20;
    FillFile( p_file->fd, size );

    constexpr int params_size = 1 << 12;
    constexpr int params_mask = params_size - 1;
    std::array< int, params_size > params;
    for (int &val: params) val = static_cast< int >( (random() * size) / RAND_MAX );

    std::function< Durations( int ) > timer = [p_file, params]( int num_iter )
        {
            TimePoints start_times = Start();
            for (int i = 0; i < num_iter; ++i)
            {
                LSeek( p_file->fd, params[i & params_mask], SEEK_SET );
            }
            return End( start_times );
        };

    return { timer, nullptr };
}


template<
    size_t size
>
static Description DescribeRW( int flags )
{

    std::ostringstream oss;
    switch (flags & O_ACCMODE)
    {
    case O_RDONLY:
        oss << "Reading";
        break;

    case O_WRONLY:
    case O_RDWR:
        oss << "Writing";
        break;

    default:
        oss << "???";
        break;
    }

    PrettyPrintSizeof( oss << " a file ", size ) << "B in length";

    if (flags & O_DIRECT) oss << " using O_DIRECT";

    oss << ".";

    Description desc;
    desc.measures = oss.str();

    return desc;
}


template<
    typename T
>
static T *Align( T *p, size_t granularity )
{
    union { T *p; ptrdiff_t i; } u = { p };
    u.i &= ~(granularity - 1);
    return u.p;
}


template<
    size_t size
>
static autotime::BenchTimers MakeReadTimers( int flags )
{
    ScopedFile writing;
    FillFile( writing.fd, size );
    if (flags & O_DIRECT) fsync( writing.fd );
    writing.close();

    std::shared_ptr< ScopedFile > p_reading =
        std::make_shared< ScopedFile >( writing.filename, flags );

    writing.filename.clear();    // clear to prevent double-unlink().

    size_t blksize = GetBlockSize( p_reading->fd );

    std::function< Durations( int ) > timer = [p_reading, blksize]( int num_iter )
        {
            // For O_DIRECT, the buffer usually needs to be aligned.
            std::vector< uint8_t > buf( size + blksize );
            uint8_t *p_data = Align( buf.data() + blksize - 1, blksize );

            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                LSeek( p_reading->fd, 0, SEEK_SET );

                TimePoints start_times = Start();
                Read( p_reading->fd, p_data, size );
                durs += End( start_times );
            }
            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::file_read_256 >()
{
    return DescribeRW< (1 << 8) >( O_RDONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_256 >()
{
    return MakeReadTimers< (1 << 8) >( O_RDONLY );
}


template<> Description Describe< Benchmark::file_read_4k >()
{
    return DescribeRW< (1 << 12) >( O_RDONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_4k >()
{
    return MakeReadTimers< (1 << 12) >( O_RDONLY );
}


template<> Description Describe< Benchmark::file_read_64k >()
{
    return DescribeRW< (1 << 16) >( O_RDONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_64k >()
{
    return MakeReadTimers< (1 << 16) >( O_RDONLY );
}


template<> Description Describe< Benchmark::file_read_1M >()
{
    return DescribeRW< (1 << 20) >( O_RDONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_1M >()
{
    return MakeReadTimers< (1 << 20) >( O_RDONLY );
}


template<> Description Describe< Benchmark::file_read_16M >()
{
    return DescribeRW< (1 << 24) >( O_RDONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_16M >()
{
    return MakeReadTimers< (1 << 24) >( O_RDONLY );
}


template<> Description Describe< Benchmark::file_read_direct_4k >()
{
    return DescribeRW< (1 << 12) >( O_RDONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_direct_4k >()
{
    return MakeReadTimers< (1 << 12) >( O_RDONLY | O_DIRECT );
}


template<> Description Describe< Benchmark::file_read_direct_64k >()
{
    return DescribeRW< (1 << 16) >( O_RDONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_direct_64k >()
{
    return MakeReadTimers< (1 << 16) >( O_RDONLY | O_DIRECT );
}


template<> Description Describe< Benchmark::file_read_direct_1M >()
{
    return DescribeRW< (1 << 20) >( O_RDONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_direct_1M >()
{
    return MakeReadTimers< (1 << 20) >( O_RDONLY | O_DIRECT );
}


template<> Description Describe< Benchmark::file_read_direct_16M >()
{
    return DescribeRW< (1 << 24) >( O_RDONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_read_direct_16M >()
{
    return MakeReadTimers< (1 << 24) >( O_RDONLY | O_DIRECT );
}


template<
    size_t size
>
static autotime::BenchTimers MakeWriteTimers( int flags )
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >( O_CREAT | flags );

    size_t blksize = GetBlockSize( p_file->fd );

    std::function< Durations( int ) > timer = [p_file, blksize]( int num_iter )
        {
            // For O_DIRECT, the buffer usually needs to be aligned.
            std::vector< uint8_t > buf( size + blksize );
            uint8_t *p_data = Align( buf.data() + blksize - 1, blksize );

            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                LSeek( p_file->fd, 0, SEEK_SET );

                TimePoints start_times = Start();
                Write( p_file->fd, p_data, size );
                durs += End( start_times );
            }
            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::file_write_256 >()
{
    return DescribeRW< (1 << 8) >( O_WRONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_256 >()
{
    return MakeWriteTimers< (1 << 8) >( O_WRONLY );
}


template<> Description Describe< Benchmark::file_write_4k >()
{
    return DescribeRW< (1 << 12) >( O_WRONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_4k >()
{
    return MakeWriteTimers< (1 << 12) >( O_WRONLY );
}


template<> Description Describe< Benchmark::file_write_64k >()
{
    return DescribeRW< (1 << 16) >( O_WRONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_64k >()
{
    return MakeWriteTimers< (1 << 16) >( O_WRONLY );
}


template<> Description Describe< Benchmark::file_write_1M >()
{
    return DescribeRW< (1 << 20) >( O_WRONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_1M >()
{
    return MakeWriteTimers< (1 << 20) >( O_WRONLY );
}


template<> Description Describe< Benchmark::file_write_16M >()
{
    return DescribeRW< (1 << 24) >( O_WRONLY );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_16M >()
{
    return MakeWriteTimers< (1 << 24) >( O_WRONLY );
}


template<> Description Describe< Benchmark::file_write_direct_4k >()
{
    return DescribeRW< (1 << 12) >( O_WRONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_direct_4k >()
{
    return MakeWriteTimers< (1 << 12) >( O_WRONLY | O_DIRECT );
}


template<> Description Describe< Benchmark::file_write_direct_64k >()
{
    return DescribeRW< (1 << 16) >( O_WRONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_direct_64k >()
{
    return MakeWriteTimers< (1 << 16) >( O_WRONLY | O_DIRECT );
}


template<> Description Describe< Benchmark::file_write_direct_1M >()
{
    return DescribeRW< (1 << 20) >( O_WRONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_direct_1M >()
{
    return MakeWriteTimers< (1 << 20) >( O_WRONLY | O_DIRECT );
}


template<> Description Describe< Benchmark::file_write_direct_16M >()
{
    return DescribeRW< (1 << 24) >( O_WRONLY | O_DIRECT );
}


template<> autotime::BenchTimers MakeTimers< Benchmark::file_write_direct_16M >()
{
    return MakeWriteTimers< (1 << 24) >( O_WRONLY | O_DIRECT );
}


} // namespace bench

