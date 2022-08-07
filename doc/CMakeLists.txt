# Note: rather than add_custom_command() and add_custom_target(),
#  the preferred approach is to use doxygen_add_docs().
#  However, since I've already got doxygen.cfg working as desired and
#  I want to use features of doxygen_add_docs() specific to a newer
#  CMake version (3.16), I'm currently sticking with this approach.
#
# See: https://cmake.org/cmake/help/latest/module/FindDoxygen.html

find_package( Doxygen )

if( DOXYGEN_FOUND )

    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg.in doxygen.cfg @ONLY )

    # TO_DO: would be better to derive this from doxygen.cfg's INPUT field.
    file( GLOB_RECURSE docs_sources
        # CONFIGURE_DEPENDS     - enable in CMake >= 3.12
        LIST_DIRECTORIES false
        "${CMAKE_SOURCE_DIR}/readme.md"
        "${CMAKE_SOURCE_DIR}/include/autotime/*.hpp"
    )

    add_custom_command(
        COMMAND "${DOXYGEN_EXECUTABLE}" doxygen.cfg > doxygen.log
        COMMENT "Generating API docs - see ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log for details."
        DEPENDS ${docs_sources}
        OUTPUT doxygen.log      # enables docs target to depend on this file
        VERBATIM
    )

    add_custom_target( docs ALL
        DEPENDS doxygen.log     # prevents the target from always running
        SOURCES ${docs_sources}
    )

    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION share/doc/autotime
        OPTIONAL
    )

else()

    message( WARNING "Doxygen not found: docs target not created." )

endif()


install(
    FILES ../readme.md
    DESTINATION share/doc/autotime
)

