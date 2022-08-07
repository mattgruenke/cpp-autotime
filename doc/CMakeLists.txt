find_package( Doxygen )

if( DOXYGEN_FOUND )

    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg.in doxygen.cfg @ONLY )

    add_custom_target( docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} doxygen.cfg > doxygen.log
        COMMENT "Generating API docs - see ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log for details."
        VERBATIM
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

