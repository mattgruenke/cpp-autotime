# Note:
#   This is a separate project, in order to test building user programs
#   against an installed copy of libautotime.  For better and more useful
#   examples using libautotime, see tools/.

cmake_minimum_required( VERSION 3.3 )

project( autotime_usertest )

find_package( AutoTime REQUIRED )

set( CMAKE_CXX_STANDARD 11 )

add_executable( standalone
    standalone.cpp
)

target_link_libraries( standalone
    AutoTime
)

