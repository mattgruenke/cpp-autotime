////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! Implements benchmarks of basic directory operations.
/*! @file

    See Category::directory, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <dirent.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/inotify.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>

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


template<> Description Describe< Category::directory >()
{
    Description desc;
    desc.measures = "Basic directory operations.";
    desc.notes = {
        "The device and filesystem measured are determined by the current working directory,"
        " when the benchmark is run.",
        "Though randomized filenames are used for tests,"
        " conflicting files will generally result in failure.",
        "Use at your own risk." };
    return desc;
}


template<> Description Describe< Benchmark::mkdir >()
{
    Description desc;
    desc.measures = "Directory creation.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::mkdir >()
{
    std::string filename = filesystem::unique_path().native();

    std::function< Durations( int ) > timer = [filename]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                TimePoints start_times = Start();
                MakeDir( filename.c_str() );
                durs += End( start_times );

                RemoveDir( filename.c_str() );
            }

            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::rmdir >()
{
    Description desc;
    desc.measures = "Directory removal.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::rmdir >()
{
    std::string filename = filesystem::unique_path().native();

    std::function< Durations( int ) > timer = [filename]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                MakeDir( filename.c_str() );

                TimePoints start_times = Start();
                RemoveDir( filename.c_str() );
                durs += End( start_times );
            }

            return durs;
        };

    return { timer, nullptr };
}


template<> Description Describe< Benchmark::chmod >()
{
    Description desc;
    desc.measures = "chmod() on a regular file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::chmod >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    p_file->close();

    std::function< void() > f = [p_file]()
        {
                // In case it matters, try to ensure the mode actually changes.
            static unsigned int count = 0;
            mode_t mode = count++ & 077;
            if (chmod( p_file->filename.c_str(), mode )) throw_system_error( errno, "chmod()" );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::chown >()
{
    Description desc;
    desc.measures = "chown() on a regular file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::chown >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();
    p_file->close();
    uid_t uid = getuid();
    gid_t gid = getgid();

    std::function< void() > f = [p_file, uid, gid]()
        {
            // Note: since this isn't actually changing the ownership,
            //  the cost might differ from when it does.
            const char *filename = p_file->filename.c_str();
            if (chown( filename, uid, gid )) throw_system_error( errno, "chown()" );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::flock >()
{
    Description desc;
    desc.measures = "flock( LOCK_SH ) + flock( LOCK_UN ) on a regular file.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::flock >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();

    std::function< void() > f = [p_file]()
        {
            if (flock( p_file->fd, LOCK_SH )) throw_system_error( errno, "flock( LOCK_SH )" );
            if (flock( p_file->fd, LOCK_UN )) throw_system_error( errno, "flock( LOCK_UN )" );
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::inotify >()
{
    Description desc;
    desc.measures = "Consuming an inotify event.";
    desc.detail = "Measures inotify event consumption, but not the time to poll for it.";
    return desc;
}


template<> autotime::BenchTimers MakeTimers< Benchmark::inotify >()
{
    std::shared_ptr< ScopedFile > p_file = std::make_shared< ScopedFile >();

    int inotify = inotify_init();
    if (inotify < 0) throw_system_error( errno, "inotify_init()" );

    int watch = inotify_add_watch( inotify, p_file->filename.c_str(), IN_MODIFY );
    if (watch < 0) throw_system_error( errno, "inotify_add_watch()" );
    
    auto p_inotify = std::make_shared< ScopedFile >( ScopedFile::make_bound( inotify ) );

    std::function< Durations( int ) > timer = [p_file, p_inotify, watch]( int num_iter )
        {
            Durations durs{};
            for (int i = 0; i < num_iter; ++i)
            {
                // Write new data to the watched file.
                if (lseek( p_file->fd, 0, SEEK_SET ) < 0) throw_system_error( errno, "lseek()" );
                const char data[] = "data";
                ssize_t written = write( p_file->fd, data, sizeof( data ) );
                if (written < 0) throw_system_error( errno, "write()" );

                // Read the expected inotify event.
                //  Don't use poll(), since event is expected and poll() is measured elsewhere.
                union
                {
                    inotify_event event;
                    char bytes[sizeof( inotify_event ) + PATH_MAX + 1];
                } buf{};
                
                TimePoints start_times = Start();
                ssize_t r = read( p_inotify->fd, &buf.bytes, sizeof( buf ) );
                durs += End( start_times );

                // Sanity-check the read results.
                if (r < 0) throw_system_error( errno, "read( inotify )" );
                assert( r >= static_cast< ssize_t >( sizeof( inotify_event ) ) );
                assert( buf.event.wd == watch );
                assert( buf.event.mask == IN_MODIFY );
            }

            return durs;
        };

    return { timer, nullptr };
}


static std::shared_ptr< std::vector< ScopedFile > > MakeFiles( size_t num )
{
    // TO_DO: make these in a subdir.
    auto p_files = std::make_shared< std::vector< ScopedFile > >();
    for (size_t i = 0; i < num; ++i)
    {
        p_files->emplace_back();
        p_files->back().close();
    }

    return p_files;
}


static size_t NumEntries = 0;


template<
    size_t size
>
static Description DescribeDirIter()
{

    std::ostringstream oss;
    PrettyPrintSizeof( oss << "directory_iterator() on ", size ) << " files.";

    Description desc;
    desc.measures = oss.str();

    return desc;
}


template<
    size_t num_files
>
static autotime::BenchTimers MakeDirIterTimers()
{
    // TO_DO: make these in a subdir.
    auto p_files = MakeFiles( num_files );

    std::function< void() > f = [p_files]()
        {
            filesystem::path dir{ "." };
#if 1
            const filesystem::directory_iterator end;
            filesystem::directory_iterator di{ dir };
            while (di != end)
            {
                NumEntries += 1;
                ++di;
            }
#else
            for (auto const &entry: filesystem::directory_iterator{ dir }) NumEntries += 1;
#endif
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::directory_iter_0 >()
{
    return DescribeDirIter< 0 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::directory_iter_0 >()
{
    return MakeDirIterTimers< 0 >();
}


template<> Description Describe< Benchmark::directory_iter_1 >()
{
    return DescribeDirIter< 1 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::directory_iter_1 >()
{
    return MakeDirIterTimers< 1 >();
}


template<> Description Describe< Benchmark::directory_iter_64 >()
{
    return DescribeDirIter< 1 << 6 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::directory_iter_64 >()
{
    return MakeDirIterTimers< 1 << 6 >();
}


template<> Description Describe< Benchmark::directory_iter_4k >()
{
    return DescribeDirIter< 1 << 12 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::directory_iter_4k >()
{
    return MakeDirIterTimers< 1 << 12 >();
}


template<
    size_t size
>
static Description DescribeReadDir()
{

    std::ostringstream oss;
    PrettyPrintSizeof( oss << "readdir() on ", size ) << " files.";

    Description desc;
    desc.measures = oss.str();

    return desc;
}


template<
    size_t num_files
>
static autotime::BenchTimers MakeReadDirTimers()
{
    // TO_DO: make these in a subdir.
    auto p_files = MakeFiles( num_files );

    std::function< void() > f = [p_files]()
        {
            filesystem::path dir{ "." };
            std::unique_ptr< DIR, int(*)(DIR *) > dp{ opendir( dir.c_str() ), &closedir };
            if (!dp) throw_system_error( errno, "opendir()" );

            while (const dirent *entry = readdir( dp.get() )) NumEntries += 1;
        };

    return { MakeTimer( f ), MakeTimer( MakeOverheadFn< void >() ) };
}


template<> Description Describe< Benchmark::readdir_0 >()
{
    return DescribeReadDir< 0 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::readdir_0 >()
{
    return MakeReadDirTimers< 0 >();
}


template<> Description Describe< Benchmark::readdir_1 >()
{
    return DescribeReadDir< 1 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::readdir_1 >()
{
    return MakeReadDirTimers< 1 >();
}


template<> Description Describe< Benchmark::readdir_64 >()
{
    return DescribeReadDir< 1 << 6 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::readdir_64 >()
{
    return MakeReadDirTimers< 1 << 6 >();
}


template<> Description Describe< Benchmark::readdir_4k >()
{
    return DescribeReadDir< 1 << 12 >();
}


template<> autotime::BenchTimers MakeTimers< Benchmark::readdir_4k >()
{
    return MakeReadDirTimers< 1 << 12 >();
}


} // namespace bench

