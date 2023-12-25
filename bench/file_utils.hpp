////////////////////////////////////////////////////////////////////////////////
//  Copyright Matthew A. Gruenke 2022.
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   http://www.boost.org/LICENSE_1_0.txt)
//
//! File-related utilities.
/*! @file

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef BENCH_FILE_UTILS_HPP
#define BENCH_FILE_UTILS_HPP


#include <fstream>
#include <string>


namespace bench
{


    //! Wraps mkdir().
    /*!
        @throws std::system_error upon failure.
    */
void MakeDir( const char *filename );


    //! Wraps rmdir().
    /*!
        @throws std::system_error upon failure.
    */
void RemoveDir( const char *filename );


    //! Wraps open().
    /*!
        @throws std::system_error upon failure.
    */
int OpenFile( const char *filename, int flags );


    //! Wraps close().
    /*!
        @throws std::system_error upon failure.
    */
void Close( int fd );


    //! Wraps unlink().
    /*!
        @throws std::system_error upon failure.
    */
void Unlink( const char *filename );


    //! Wraps lseek().
    /*!
        @throws std::system_error upon failure.
    */
size_t LSeek( int fd, ssize_t offset, int whence );

    //! Wraps read(), looping until all requested bytes are read.
    /*!
        @throws std::system_error upon failure.
    */
void Read( int fd, void *buf, size_t count );

    //! Wraps read().
    /*!
        @throws std::system_error upon failure.

        @note
        Returns 0, in case of EINTR or EAGAIN.
    */
size_t ReadSome( int fd, void *buf, size_t count );

    //! Wraps write(), looping until all bytes are written.
    /*!
        @throws std::system_error upon failure.
    */
void Write( int fd, const void *buf, size_t count );


    //! Wraps write().
    /*!
        @throws std::system_error upon failure.

        @returns the number of bytes written.

        @note
        Returns 0, in case of EINTR or EAGAIN.
    */
size_t WriteSome( int fd, const void *buf, size_t count );


    //! Fills the file with random data.
    /*!
        @throws std::system_error upon failure.

        @note
        This performs multiple writes and does some extra work, during the
        first call.
    */
void FillFile( int fd, size_t size );


    //! Default supplementary mode bits value (none).
static constexpr std::ios_base::openmode OpenMode0{};


    //! Opens a file for input, enabling std::ios_base::failure exceptions.
    /*!
        @throws std::ios_base::failure if file couldn't be opened for input.

        The mode parameter is for specifying bits besides std::ios_base::in,
        if desired.  Typically, this would just be std::ios_base::binary.
    */
std::ifstream OpenIFStream(
    const std::string &filename,                //!< Absolute or relative path.
    std::ifstream::openmode mode = OpenMode0    //!< Additional mode bits.
);


    //! Reads a single value from the specified file.
    /*!
        @throws std::ios_base::failure
            if file couldn't be opened for input or read fails.
    */
template<
    typename value_type
>
value_type ReadFromFile(
    const std::string &filename                 //!< Absolute or relative path.
)
{
    value_type value;
    std::ifstream stream = OpenIFStream( filename );
    stream >> value;
    return value;
}


    //! Binds a file & its handle to a scope.
    /*!
        Upon construction, the file is opened (and created, if necessary).
        Upon destruction, it's closed (if still open) and unlinked.
        Thus, passing the name of an existing file will result in its
        eventual deletion.  Otherwise, instantiating the class will result
        in an entirely emphemeral (but, otherwise ordinary) file.

        @throws std::system_error upon failure.
    */
struct ScopedFile
{
    static const int default_flags;     // O_CREAT | O_RDWR

    const int flags = default_flags;
    std::string filename;
    int fd = -1;

        //! Instantiates with a uniquely-named file in the CWD.
    explicit ScopedFile( int flags = default_flags );

        //! Opens the specified filename for read/write, creating if necessary.
    explicit ScopedFile(
        const std::string &filename, int flags = default_flags );

        //! Closes (if open) and unlinks the file.
    ~ScopedFile();

        //! Closes the file (if open).
    void close();
};


} // namespace bench


#endif  // ndef BENCH_FILE_UTILS_HPP

