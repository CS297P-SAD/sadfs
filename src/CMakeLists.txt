# src/CMakeLists.txt

add_compile_options( -ggdb )

# FindgRPC.cmake module location where function grpc_generate_cpp() is defined
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
find_package (Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# directory containing all *.pb.h and *.pb.grpc.h files so they can be included
set (BUILD_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

# generate *.pb.cc and *.pb.h files
protobuf_generate_cpp (PROTO_SOURCE_FILES PROTO_HEADER_FILES
	proto/common.proto
	proto/master.proto
	proto/chunk.proto
	proto/internal.proto)
# compile *.pb.cc files into a library
add_library (sadfs_protobuf STATIC ${PROTO_SOURCE_FILES})
target_include_directories (sadfs_protobuf PUBLIC
	${Protobuf_INCLUDE_DIRS}
	${BUILD_INCLUDE_DIR})
target_link_libraries (sadfs_protobuf
	${Protobuf_LIBRARIES})
# copy *.pb.h into BUILD_INCLUDE_DIR before the library is built so that they
# can actually be #include-d
set (PROTO_HEADER_FILES_DIR ${BUILD_INCLUDE_DIR}/sadfs/proto)
add_custom_target (sadfs_proto_includes
	DEPENDS ${PROTO_HEADER_FILES}
	COMMAND ${CMAKE_COMMAND} -E
	make_directory ${PROTO_HEADER_FILES_DIR}
	COMMAND ${CMAKE_COMMAND} -E
	copy ${PROTO_HEADER_FILES} ${PROTO_HEADER_FILES_DIR})
add_dependencies (sadfs_protobuf sadfs_proto_includes)

# generate *.grpc.pb.cc and *.grpc.pb.h files
grpc_generate_cpp (GRPC_SOURCE_FILES GRPC_HEADER_FILES
   	grpc/services.proto)
# compile *.pb.grpc.cc files into a library
add_library (sadfs_grpc STATIC ${GRPC_SOURCE_FILES})
target_include_directories (sadfs_grpc PUBLIC
	${gRPC_INCLUDE_DIR}
	${BUILD_INCLUDE_DIR})
target_link_libraries (sadfs_grpc
	gRPC::grpc++)
# copy *.pb.h into BUILD_INCLUDE_DIR before the library is built so that they
# can actually be #include-d
set (GRPC_HEADER_FILES_DIR ${BUILD_INCLUDE_DIR}/sadfs/grpc)
add_custom_target (sadfs_grpc_includes
	DEPENDS ${GRPC_HEADER_FILES}
	COMMAND ${CMAKE_COMMAND} -E
	make_directory ${GRPC_HEADER_FILES_DIR}
	COMMAND ${CMAKE_COMMAND} -E
	copy ${GRPC_HEADER_FILES} ${GRPC_HEADER_FILES_DIR})
add_dependencies (sadfs_grpc sadfs_grpc_includes)

# common components that almost all targets depend on
set (COMMON_SOURCE_FILES
	comm/inet.cpp comm/socket.cpp
	msgs/master.cpp msgs/chunk.cpp msgs/channel.cpp)

# add main targets
foreach (TARGET sadmd sadcd)
	add_executable (${TARGET}
		${TARGET}/main.cpp
		${TARGET}/${TARGET}.cpp
		${COMMON_SOURCE_FILES})
endforeach()

# first, find the boost_program_options library. this sets
# the Boost_INCLUDE_DIRS and Boost_LIBRARIES variables
find_package (Boost
	1.57.0
	REQUIRED
	COMPONENTS program_options)

# then, do the following for every main daemon target
# 1. add a corresponding bootstrap target
# 2. inform CMake of any external dependencies
foreach (TARGET sadmd sadcd sadfsd)
	set (BOOTSTRAP_TARGET ${TARGET}-bootstrap)
	add_executable (${BOOTSTRAP_TARGET}
		${TARGET}/${BOOTSTRAP_TARGET}.cpp bootstrap/util.cpp)
	target_include_directories (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_INCLUDE_DIRS})
	target_link_libraries (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_LIBRARIES})
endforeach()

# source files for sadfsd-bootstrap
target_sources (sadfsd-bootstrap
	PRIVATE sadfsd/sadfilesys.cpp
	PRIVATE ${COMMON_SOURCE_FILES})

# for sadmd and sadcd, mark their bootstrap targets as dependencies
foreach (TARGET sadmd sadcd)
	add_dependencies (${TARGET} ${TARGET}-bootstrap)
endforeach()

# link with the sadfs_protobuf library
foreach (TARGET sadmd sadcd sadfsd-bootstrap)
	target_link_libraries (${TARGET} PRIVATE sadfs_protobuf)
endforeach()

# link with the sadfs_grpc library
foreach (TARGET sadcd sadfsd-bootstrap)
	target_link_libraries (${TARGET} PRIVATE sadfs_grpc)
endforeach()

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
target_sources (echod     PRIVATE comm/inet.cpp comm/socket.cpp)
target_sources (msg       PRIVATE comm/inet.cpp comm/socket.cpp)
target_sources (proto-msg PRIVATE ${COMMON_SOURCE_FILES})
target_link_libraries (proto-msg PRIVATE sadfs_protobuf)
target_include_directories (uuid PRIVATE ${Boost_INCLUDE_DIRS})
