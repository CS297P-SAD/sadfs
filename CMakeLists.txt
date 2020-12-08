# top level CMakeLists.txt
cmake_minimum_required (VERSION 3.13.x)
project (sadfs)

# enforce c++17
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# set output directory for binaries
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# we want all targets to be able to find headers
include_directories (${CMAKE_SOURCE_DIR}/include)

# delegate config to sub-directories
# dependent components must appear first
add_subdirectory (example)
add_subdirectory (src)
