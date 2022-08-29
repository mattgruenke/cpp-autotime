set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


find_package( Boost
    COMPONENTS REQUIRED
        program_options
)


add_executable( sysinfo
    sysinfo.cpp
)

target_link_libraries( sysinfo
    autotime
)


install( TARGETS sysinfo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

