cmake_minimum_required(VERSION 3.5)

project("GI learner")

# Enable all warning by default
set(CMAKE_CXX_FLAGS "-g -Wall")

################################
# Normal Libraries & Executables
################################
include_directories(gi)

add_executable(gilearner main.cc)

# Extra linking for the project.
target_link_libraries(gilearner gi_lib)

################################
# Testing
################################
if (test)
  if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
  endif (APPLE)

  # This adds another subdirectory, which has the gilib library'.
  add_subdirectory(gi)

  # This adds another subdirectory, which has 'project(googletest)'.
  add_subdirectory(googletest/googletest)

  enable_testing()

  # Include the gtest library. gtest_SOURCE_DIR is available due to
  # 'project(gtest)' above.
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  ##############
  # Unit Tests
  ##############
  add_executable(runUnitTests test_dfa.cc)

  # Standard linking to gtest stuff.
  target_link_libraries(runUnitTests gtest gtest_main)

  # Extra linking for the project.
  target_link_libraries(runUnitTests gi_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  add_test(NAME that-test-I-made COMMAND runUnitTests)

  # You can also omit NAME and COMMAND. The second argument could be some other
  # test executable.
  add_test(that-other-test-I-made runUnitTests)
endif()
