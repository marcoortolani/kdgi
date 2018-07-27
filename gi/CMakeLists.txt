find_package(Boost 1.58 REQUIRED)

include_directories(Boost_INCLUDE_DIRS)

# MODIFICA SERVER: ho escluso bluestar.cc
# Andrea 27/07/18
add_library(gi_lib concretedfa.cc utilities.cc dfastate.cc redbluedfa.cc bluefringe.cc rpni.cc edsm.cc)
