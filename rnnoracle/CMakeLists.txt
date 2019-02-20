#Needed because if python=on the pybind11 library is already included.
if (NOT python)
	add_subdirectory(../py/pybind11 lib)
endif()

add_library(rnnoracle rnnoracle.cc)
find_package( PythonInterp REQUIRED )
find_package( PythonLibs REQUIRED )
#find_package(PythonInterp 3.6 REQUIRED)
#find_package(PythonLibs 3.6 REQUIRED)
target_link_libraries(rnnoracle pybind11)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(rnnoracle ${PYTHON_LIBRARIES})
