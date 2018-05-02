find_package(Boost 1.58 REQUIRED)

include_directories(Boost_INCLUDE_DIRS)

add_library(gi_lib dfa.cc utilities.cc rpni.cc bluefringe.cc redbluedfa.cc edsm.cc bluestar.cc)
