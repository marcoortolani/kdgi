cmake_minimum_required(VERSION 3.5)

project("GI learner")

# Enable all warning by default
set(CMAKE_CXX_FLAGS "-g -Wall")

set (CMAKE_CXX_STANDARD 11)

################################
# Normal Libraries & Executables
################################
include_directories(gi)

# This adds another subdirectory, which has the gilib library'.
add_subdirectory(gi)

add_executable(gilearner main.cc)

# Extra linking for the project.
target_link_libraries(gilearner gi_lib)

################################
# Python
################################
if (python)
#  find_package (Python)
  find_package( PythonInterp 2.7 REQUIRED )
  find_package( PythonLibs 2.7 REQUIRED )
  add_subdirectory(py)
endif()

################################
# Testing
################################
if (test)

  # Testing through Googletest; addedd as a submodule
  # to this git repository
  # git submodule add https://github.com/google/googletest.git gtest
  include(gtest.cmake)

  # GTest needs threading support
  find_package (Threads)
  target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(gtest_main ${CMAKE_THREAD_LIBS_INIT})

  if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
  endif (APPLE)

  enable_testing()

  ##############
  # Unit Tests
  ##############
  #dare chmod 777 su ogni file
  add_executable(runUnitTests unit_testing/base_dfa_test.cc)

  # Standard linking to gtest stuff.
  target_link_libraries(runUnitTests gtest gtest_main)

  # Extra linking for the project.
  target_link_libraries(runUnitTests gi_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  add_test(NAME DfaTestRoutines COMMAND runUnitTests)

  # You can also omit NAME and COMMAND. The second argument could be some other
  # test executable.
  # add_test(that-other-test-I-made runUnitTests)

endif()
