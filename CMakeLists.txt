cmake_minimum_required( VERSION 3.3 )

project( autotime )

include( GNUInstallDirs )

option( BUILD_TOOLS "Enable compilation of tools executables" YES )
option( BUILD_DOCS  "Enable generation of API documentation"  YES )

add_subdirectory( cmake )
add_subdirectory( lib )

if( BUILD_DOCS )
    add_subdirectory( doc )
endif()

if( BUILD_TOOLS )
    add_subdirectory( bench )
    add_subdirectory( tools )
endif()

