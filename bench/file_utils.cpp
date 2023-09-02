////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//! File-related utilities.
/*! @file

    See file_utils.hpp, for details.
*/
////////////////////////////////////////////////////////////////////////////////////////////////

#include "file_utils.hpp"

#include <array>

#include <fcntl.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

#include "error_utils.hpp"


namespace filesystem = boost::filesystem;

namespace bench
{


int OpenFile( const char *filename, int flags )
{
    constexpr mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    int fd = open( filename, flags, mode );
    if (fd < 0) throw_system_error( errno, "open()" );

    return fd;
}


void Close( int fd )
{
    if (fd >= 0 && close( fd )) throw_system_error( errno, "close()" );
}


void Unlink( const char *filename )
{
    if (unlink( filename )) throw_system_error( errno, "unlink()" );
}


size_t LSeek( int fd, ssize_t offset, int whence )
{
    off_t ofs = lseek( fd, offset, whence );
    if (ofs < 0) throw_system_error( errno, "lseek()" );
    return static_cast< size_t >( ofs );
}


void Read( int fd, void *buf, size_t count )
{
    size_t ofs = 0;
    while (ofs < count) ofs += ReadSome( fd, (uint8_t *) buf + ofs, count - ofs );
}


size_t ReadSome( int fd, void *buf, size_t count )
{
    ssize_t rtn = read( fd, buf, count );
    if (rtn < 0 && errno != EAGAIN && errno != EINTR) throw_system_error( errno, "read()" );

    return static_cast< size_t >( std::max< ssize_t >( 0, rtn ) );
}


void Write( int fd, const void *buf, size_t count )
{
    size_t ofs = 0;
    while (ofs < count) ofs += WriteSome( fd, (uint8_t *) buf + ofs, count - ofs );
}


size_t WriteSome( int fd, const void *buf, size_t count )
{
    ssize_t written = write( fd, buf, count );
    if (written < 0)
    {
        if (errno == EAGAIN || errno == EINTR) written = 0;
        else throw_system_error( errno, "write()" );
    }

    return static_cast< size_t >( written );
}


static constexpr size_t Data_size = 4096;


static std::array< uint8_t, Data_size > MakeData()
{
    std::array< uint8_t, Data_size > array;
    for (uint8_t &element: array) element = static_cast< uint8_t >( random() & 0xff );

    return array;
}


void FillFile( int fd, size_t size )
{
    static const auto array = MakeData();
    size_t remain = size;
    while (remain > 0) remain -= WriteSome( fd, array.data(), std::min( array.size(), remain ) );
}


std::ifstream OpenIFStream( const std::string &filename, std::ifstream::openmode mode )
{
    std::ifstream stream;
    stream.exceptions( std::ifstream::badbit | std::ifstream::failbit );
    stream.open( filename, mode | std::ifstream::in );
    return stream;
}


// struct ScopedFile:
const int ScopedFile::default_flags = O_CREAT | O_RDWR;


ScopedFile::ScopedFile( int flags )
:
    ScopedFile( filesystem::unique_path().native(), flags )
{
}


ScopedFile::ScopedFile( const std::string &filename, int flags )
:
    flags( flags ),
    filename( filename ),
    fd( OpenFile( filename.c_str(), flags ) )
{
}


ScopedFile::~ScopedFile()
{
    this->close();
    if (!filename.empty()) Unlink( filename.c_str() );
}


void ScopedFile::close()
{
    if (fd >= 0) Close( fd );
    fd = -1;
}


} // namespace bench

