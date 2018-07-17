find_package(Boost 1.58 REQUIRED)

include_directories(Boost_INCLUDE_DIRS)

<<<<<<< HEAD
add_library(gi_lib concretedfa.cc utilities.cc angluindfa.cc angluinlearner.cc sillyoracle.cc dfastate.cc)
=======
add_library(gi_lib concretedfa.cc utilities.cc dfastate.cc)
>>>>>>> origin/master
