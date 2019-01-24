cmake_minimum_required(VERSION 3.5)

# Enable all warning by default
#set(CMAKE_CXX_FLAGS "-g -Wall -fvisibility=hidden -fPIC")

set(CMAKE_CXX_STANDARD 11)

#add_executable(hello hello.cc)

add_library(lstm lstm.cc)

find_package(PythonLibs REQUIRED)
#include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(lstm ${PYTHON_LIBRARIES})
#include_directories("/usr/include/python3.6m/")
#target_link_libraries(lstm "/home/marco/anaconda3/envs/alearn/lib/libpython3.6m.so")


#Needed to copy some files in the build folder
#add_custom_command(TARGET hello PRE_BUILD
#					COMMAND ${CMAKE_COMMAND} -E copy_directory
#					${CMAKE_SOURCE_DIR}/ker $<TARGET_FILE_DIR:hello>)
