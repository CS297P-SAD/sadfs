# src/CMakeLists.txt
# add sub-directories that need to declare targets
add_subdirectory (sadmd)
add_subdirectory (sadcd)
# add_subdirectory (sadfsd)
add_subdirectory (bootstrap)

# common components that others depend on
set (COMMON_SOURCES comm/inet.cpp comm/socket.cpp)

# declare dependencies for relevant targets
target_sources (sadmd  PRIVATE ${COMMON_SOURCES})
target_sources (sadcd  PRIVATE ${COMMON_SOURCES})
# target_sources (sadfsd PRIVATE ${COMMON_SOURCES})
target_sources (echod  PRIVATE ${COMMON_SOURCES})
target_sources (msg    PRIVATE ${COMMON_SOURCES})
