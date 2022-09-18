set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


find_package( Boost
    COMPONENTS REQUIRED
        program_options
)


find_package( Threads REQUIRED )


add_executable( autotime-bench
    atomic_benchmarks.cpp
    dispatch.cpp
    list.cpp
    main.cpp
    memory_benchmarks.cpp
    mutex_benchmarks.cpp
    output.cpp
    overhead_benchmarks.cpp
    thread_benchmarks.cpp
    thread_utils.cpp
    work_benchmarks.cpp
)

target_link_libraries( autotime-bench
    autotime
    Boost::program_options
    Threads::Threads
)


install( TARGETS autotime-bench
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

