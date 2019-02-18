cmake_minimum_required(VERSION 3.5)

# Enable all warning by default
#set(CMAKE_CXX_FLAGS "-g -Wall -fvisibility=hidden -fPIC")

set(CMAKE_CXX_STANDARD 11)

add_library(rnnoracle rnnoracle.cc)

find_package(PythonLibs REQUIRED)
#include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(rnnoracle ${PYTHON_LIBRARIES})

target_link_libraries(rnnoracle gi_lib)

#include_directories("/usr/include/python3.6m/")
#target_link_libraries(lstm "/home/marco/anaconda3/envs/alearn/lib/libpython3.6m.so")
