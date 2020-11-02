# top level CMakeLists.txt
cmake_minimum_required (VERSION 3.13.x)
project (SADFS)

# enfore c++17
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# delegate config to sub-directories
# dependent components must appear first
add_subdirectory (example)
add_subdirectory (src)
# add_subdirectory (proto)
