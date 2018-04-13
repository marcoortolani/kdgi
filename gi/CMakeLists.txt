find_package(Boost 1.54 REQUIRED)

include_directories(Boost_INCLUDE_DIRS)

add_library(gi_lib dfa.cc utilities.cc)
