add_library( autotime SHARED
    log.cpp
    os.cpp
    work.cpp
)

target_include_directories( autotime PUBLIC ../include )

    # Unfortunately, this flattens the hierarchy.
#file( GLOB headers
#    LIST_DIRECTORIES false
#    ../include/*
#    ../include/autotime/*
#    ../include/autotime/detail/*
#)

set_target_properties( autotime PROPERTIES
#    PUBLIC_HEADER "${headers}"
    CXX_STANDARD 11
)

install( TARGETS autotime
    LIBRARY             DESTINATION lib
#    PUBLIC_HEADER      DESTINATION include
)

    # TO_DO: switch to target_sources( FILE_SET ) in CMake 3.23
install( DIRECTORY ../include
    DESTINATION .
)

#if( BUILD_TESTING )
#    add_subdirectory( tests )
#endif()

