add_library( autotime SHARED
    log.cpp
    os.cpp
    work.cpp
)

target_include_directories( autotime PUBLIC ../include )

set_target_properties( autotime PROPERTIES
    CXX_STANDARD 11
)

install( TARGETS autotime
    LIBRARY             DESTINATION lib
)

    # TO_DO: switch to target_sources( FILE_SET ) in CMake 3.23
install( DIRECTORY ../include
    DESTINATION .
)

