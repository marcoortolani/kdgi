# find_package(Boost 1.58 REQUIRED)
# include_directories(Boost_INCLUDE_DIRS)
# add_library(gipy_lib gipy.cc)

add_subdirectory(pybind11)

pybind11_add_module(gipy_lib gipy.cc)

set_target_properties(gipy_lib PROPERTIES SUFFIX ".so")

target_link_libraries(gipy_lib PUBLIC gi_lib)
