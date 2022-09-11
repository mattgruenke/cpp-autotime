set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


find_package( Boost
    COMPONENTS REQUIRED
        program_options
)


add_executable( autotime-bench
    dispatch.cpp
    list.cpp
    main.cpp
    output.cpp
)

target_link_libraries( autotime-bench
    autotime
    Boost::program_options
)


install( TARGETS autotime-bench
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

