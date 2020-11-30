# src/CMakeLists.txt

# compile protobuf files into a library
add_compile_options( -ggdb )
find_package (Protobuf REQUIRED)
protobuf_generate_cpp (PROTO_SOURCE_FILES PROTO_HEADER_FILES
	comm/messages.proto
	comm/internal.proto)
add_library (sadfs_protobuf STATIC ${PROTO_SOURCE_FILES})
# directory containing all *.pb.h so they can be included
set (PROTO_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/)
target_include_directories (sadfs_protobuf PUBLIC
	${Protobuf_INCLUDE_DIRS}
	${PROTO_INCLUDE_DIR})
target_link_libraries (sadfs_protobuf
	${Protobuf_LIBRARIES})
# copy *.pb.h into PROTO_INCLUDE_DIR before the library is built
# so that they can actually be #include-d
set (PROTO_HEADER_FILES_DIR ${PROTO_INCLUDE_DIR}sadfs/proto/)
add_custom_target (sadfs_protobuf_includes
	DEPENDS ${PROTO_HEADER_FILES}
	COMMAND ${CMAKE_COMMAND} -E
	make_directory ${PROTO_HEADER_FILES_DIR}
	COMMAND ${CMAKE_COMMAND} -E
	copy ${PROTO_HEADER_FILES} ${PROTO_HEADER_FILES_DIR})
add_dependencies (sadfs_protobuf sadfs_protobuf_includes)
# add sadfs's protobuf abstraction component(s)
target_sources (sadfs_protobuf PRIVATE
	comm/messages.cpp)

# common components that almost all targets depend on
set (COMMON_SOURCE_FILES
	comm/inet.cpp comm/socket.cpp)
# add main targets
add_executable (sadmd
	sadmd/main.cpp
	sadmd/sadmd.cpp
	${COMMON_SOURCE_FILES})
add_executable (sadcd
	sadcd/main.cpp
	sadcd/sadcd.cpp
	${COMMON_SOURCE_FILES})

# first, find the boost_program_options library. this sets
# the Boost_INCLUDE_DIRS and Boost_LIBRARIES variables
find_package (Boost
	1.57.0
	REQUIRED
	COMPONENTS program_options)

# then, do the following for every main daemon target
# 1. add a corresponding bootstrap target
# 2. inform CMake of dependency on protobuf headers
# 3. inform CMake of any external dependencies
foreach (TARGET sadmd sadcd sadfsd)
	set (BOOTSTRAP_TARGET ${TARGET}-bootstrap)
	add_executable (${BOOTSTRAP_TARGET}
		${TARGET}/${BOOTSTRAP_TARGET}.cpp bootstrap/util.cpp)
	target_link_libraries (${BOOTSTRAP_TARGET}
		PRIVATE sadfs_protobuf)
	target_include_directories (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_INCLUDE_DIRS})
	target_link_libraries (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_LIBRARIES})
endforeach()

# for sadmd and sadcd, do the following
# 1. mark their bootstrap targets as dependencies
# 2. mark sadfs_protobuf as a dependency
foreach(TARGET sadmd sadcd)
	add_dependencies (${TARGET} ${TARGET}-bootstrap)
	target_link_libraries (${TARGET} PRIVATE sadfs_protobuf)
endforeach()

# source files for sadfsd-bootstrap
target_sources (sadfsd-bootstrap
    PRIVATE sadfsd/sadfilesys.cpp
    PRIVATE ${COMMON_SOURCE_FILES})

# find fuse library. this sets the FUSE_INCLUDE_DIRS and FUSE_LIBRARIES
# variables
find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse)

target_include_directories (sadfsd-bootstrap
    SYSTEM PRIVATE ${FUSE_INCLUDE_DIRS})
target_link_libraries (sadfsd-bootstrap
    PRIVATE ${FUSE_LINK_LIBRARIES})
target_compile_options(sadfsd-bootstrap
    PRIVATE ${FUSE_CFLAGS})

# find the SQLite3 library. this sets
# the SQLite3_INCLUDE_DIRS and SQLite3_LIBRARIES variables
find_package (SQLite3
	REQUIRED)

foreach (TARGET sadmd) # sadcd
	target_include_directories (${TARGET}
		PRIVATE ${SQLite3_INCLUDE_DIRS})
	target_link_libraries (${TARGET}
		PRIVATE ${SQLite3_LIBRARIES})
endforeach()

# add source files for "example" targets
target_sources (echod        PRIVATE ${COMMON_SOURCE_FILES})
target_sources (msg          PRIVATE ${COMMON_SOURCE_FILES})
target_sources (proto-msg    PRIVATE ${COMMON_SOURCE_FILES})
target_link_libraries (proto-msg PRIVATE sadfs_protobuf)
