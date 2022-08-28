## Finds AutoTime
#
# Sets:
#   AutoTime_INCLUDE_DIRS - libautotime include path.
#   AutoTime_LIBRARIES - libraries to link for libautotime.
#   AutoTime_FOUND - Set if the dependencies were found.
#   AutoTime_VERSION - indicates version of libautotime.

include( GNUInstallDirs )

find_path( AutoTime_INCLUDE_DIR autotime/version.hpp
    HINTS "${CMAKE_CURRENT_LIST_DIR}/../../../${CMAKE_INSTALL_INCLUDEDIR}"
)

find_library( AutoTime_LIBRARY libautotime.so
    HINTS "${CMAKE_CURRENT_LIST_DIR}/../../../${CMAKE_INSTALL_LIBDIR}"
)

set( AUTOTIME_VERSION_HPP ${AutoTime_INCLUDE_DIR}/autotime/version.hpp )
if( EXISTS ${AUTOTIME_VERSION_HPP} )

    function( AUTOTIME_GET_VER result part )
        file( STRINGS ${AUTOTIME_VERSION_HPP} LINE
            REGEX "#define[ \t]+AUTOTIME_VERSION_${part}" )
        if( LINE MATCHES "#define[ \t]+AUTOTIME_VERSION_${part}[ \t]+([0-9]+)" )
            set( ${result} "${CMAKE_MATCH_1}" PARENT_SCOPE )
        endif()
    endfunction()

    AUTOTIME_GET_VER( AUTOTIME_VER_MAJOR MAJOR )
    AUTOTIME_GET_VER( AUTOTIME_VER_MINOR MINOR )
    AUTOTIME_GET_VER( AUTOTIME_VER_PATCH PATCH )

    set( AutoTime_VERSION "${AUTOTIME_VER_MAJOR}.${AUTOTIME_VER_MINOR}.${AUTOTIME_VER_PATCH}" )

endif()

include( FindPackageHandleStandardArgs )

find_package_handle_standard_args( AutoTime
    FOUND_VAR AutoTime_FOUND
    REQUIRED_VARS AutoTime_LIBRARY AutoTime_INCLUDE_DIR
    VERSION_VAR AutoTime_VERSION
)

if( AutoTime_FOUND )

    set( AutoTime_LIBRARIES ${AutoTime_LIBRARY} )
    set( AutoTime_INCLUDE_DIRS ${AutoTime_INCLUDE_DIR} )

    if( NOT TARGET AutoTime )
        add_library( AutoTime UNKNOWN IMPORTED )
        set_target_properties( AutoTime PROPERTIES
            IMPORTED_LOCATION "${AutoTime_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${AutoTime_INCLUDE_DIRS}"
        )
    endif()

endif()

mark_as_advanced(
    AutoTime_INCLUDE_DIR
    AutoTime_LIBRARY
)

