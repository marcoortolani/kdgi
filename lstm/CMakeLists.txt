add_subdirectory(../py/pybind11 lib)

add_library(lstm lstm.cc)

find_package(PythonLibs 3.6 REQUIRED)
find_package(PythonInterp 3.6 REQUIRED)
target_link_libraries(lstm pybind11)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(lstm ${PYTHON_LIBRARIES})

message("messaggio di test python directories ${PYTHON_INCLUDE_DIRS} python libraries ${PYTHON_LIBRARIES}")
