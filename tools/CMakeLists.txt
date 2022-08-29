set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


add_executable( sysinfo
    sysinfo.cpp
)

target_link_libraries( sysinfo
    autotime
)

