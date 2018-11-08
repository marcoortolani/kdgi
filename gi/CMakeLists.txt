#########
# BOOST #
#########
find_package(Boost 1.58 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# MESSAGE(STATUS "Include dirs >>>${Boost_INCLUDE_DIRS}<<<")
# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()

# MODIFICA SERVER: ho escluso bluestar.cc
# Andrea 27/07/18
add_library(gi_lib concretedfa.cc utilities.cc dfastate.cc redbluedfa.cc bluefringe.cc rpni.cc edsm.cc)
