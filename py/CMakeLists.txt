# find_package(Boost 1.58 REQUIRED)
# include_directories(Boost_INCLUDE_DIRS)
# add_library(gipy_lib gipy.cc)

add_subdirectory(pybind11)

pybind11_add_module(gipy_lib gipy.cc)

target_link_libraries(gipy_lib PUBLIC gi_lib)
