set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


add_executable( autotime-sysinfo
    sysinfo.cpp
)

target_link_libraries( autotime-sysinfo
    autotime
)


install( TARGETS autotime-sysinfo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

