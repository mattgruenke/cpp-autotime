set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


find_package( Boost
    COMPONENTS REQUIRED
        filesystem
        program_options
        system
)


find_package( Threads REQUIRED )


add_executable( autotime-bench
    asio_benchmarks.cpp
    atomic_benchmarks.cpp
    condvar_benchmarks.cpp
    container_benchmarks.cpp
    container_utils.cpp
    description.cpp
    directory_benchmarks.cpp
    dispatch.cpp
    exception_benchmarks.cpp
    exception_utils.cpp
    file_benchmarks.cpp
    file_utils.cpp
    function_benchmarks.cpp
    function_utils.cpp
    hash_benchmarks.cpp
    heap_benchmarks.cpp
    list.cpp
    main.cpp
    memory_benchmarks.cpp
    mutex_benchmarks.cpp
    output.cpp
    overhead_benchmarks.cpp
    pipe_benchmarks.cpp
    pipe_utils.cpp
    poll_benchmarks.cpp
    stream_benchmarks.cpp
    thread_benchmarks.cpp
    thread_utils.cpp
    work_benchmarks.cpp
)

target_compile_options( autotime-bench PRIVATE
    -Wextra
#    -Wno-unused-parameter
#    -O0
)

target_link_libraries( autotime-bench
    autotime
    Boost::filesystem
    Boost::program_options
    Boost::system
    Threads::Threads
)


install( TARGETS autotime-bench
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

