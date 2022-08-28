add_library( autotime SHARED
    autotime.cpp
    estimate.cpp
    internal.cpp
    iterate.cpp
    log.cpp
    os.cpp
    time.cpp
    types.cpp
    warmup.cpp
    work.cpp
)

target_include_directories( autotime PUBLIC ../include )

set_target_properties( autotime PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

install( TARGETS autotime
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

    # TO_DO: switch to target_sources( FILE_SET ) in CMake 3.23
install( DIRECTORY ../include
    DESTINATION .
)

