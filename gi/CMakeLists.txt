find_package(Boost 1.58 REQUIRED)

include_directories(Boost_INCLUDE_DIRS)

add_library(gi_lib concretedfa.cc utilities.cc angluindfa.cc angluinlearner.cc sillyoracle.cc dfastate.cc)
