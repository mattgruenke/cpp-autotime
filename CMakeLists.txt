cmake_minimum_required( VERSION 3.3 )

project( autotime )

include( GNUInstallDirs )

option( BUILD_TOOLS "Enable compilation of tools executables" YES )

add_subdirectory( cmake )
add_subdirectory( lib )

if( BUILD_TOOLS )
    add_subdirectory( tools )
endif()

