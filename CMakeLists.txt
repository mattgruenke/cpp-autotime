cmake_minimum_required( VERSION 3.3 )

project( autotime )

include( GNUInstallDirs )

option( BUILD_TOOLS "Enable compilation of tools executables" YES )
option( BUILD_DOCS  "Enable generation of API documentation"  YES )

#set( ARCH_FLAGS "-march=native" )
#set( PERF_FLAGS "-fomit-frame-pointer -Ofast" )
set( WARN_FLAGS "-Wall -Wextra -Wabi -Wc++14-compat" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb ${ARCH_FLAGS} ${PERF_FLAGS} ${WARN_FLAGS}" )

add_subdirectory( cmake )
add_subdirectory( lib )

if( BUILD_DOCS )
    add_subdirectory( doc )
endif()

if( BUILD_TOOLS )
    add_subdirectory( bench )
    add_subdirectory( tools )
endif()

