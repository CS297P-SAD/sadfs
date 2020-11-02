# src/CMakeLists.txt
# add sub-directories that need to declare targets
add_subdirectory (sadmd)
# add_subdirectory (sadcd)
# add_subdirectory (sadfsd)
add_subdirectory (bootstrap)

# common components that others depend on
set (COMMON_SOURCES comm/inet.cpp comm/socket.cpp)

# declare dependencies for relevant targets
target_sources (sadmd  PRIVATE ${COMMON_SOURCES})
target_sources (sadcd  PRIVATE ${COMMON_SOURCES})
target_sources (sadfsd PRIVATE ${COMMON_SOURCES})
target_sources (echod  PRIVATE ${COMMON_SOURCES})
target_sources (msg    PRIVATE ${COMMON_SOURCES})

# expose header files for example programs
target_include_directories (echod PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (msg   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
