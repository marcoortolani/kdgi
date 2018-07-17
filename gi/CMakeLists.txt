find_package(Boost 1.58 REQUIRED)

include_directories(Boost_INCLUDE_DIRS)

add_library(gi_lib concretedfa.cc utilities.cc dfastate.cc redbluedfa.cc bluefringe.cc rpni.cc edsm.cc bluestar.cc)
