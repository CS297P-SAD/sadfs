# src/CMakeLists.txt
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
# add_executable (sadfsd
#	sadfsd/sadfsd.cpp
#	${COMMON_SOURCE_FILES})

# define a function to do the following for every main daemon target
# 1. add a corresponding bootstrap target
# 2. mark it as a dependency of the main target
# 3. inform CMake of any external dependencies
function (add_bootstrap TARGET)
	set (BOOTSTRAP_TARGET ${TARGET}-bootstrap)
	add_executable (${BOOTSTRAP_TARGET}
		${TARGET}/${BOOTSTRAP_TARGET}.cpp bootstrap/util.cpp)
	add_dependencies (${TARGET} ${BOOTSTRAP_TARGET})
	target_include_directories (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_INCLUDE_DIRS})
	target_link_libraries (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_LIBRARIES})
endfunction()

# first, find the boost_program_options library. this sets
# the Boost_INCLUDE_DIRS and Boost_LIBRARIES variables
find_package (Boost
	1.57.0
	REQUIRED
	COMPONENTS program_options)

add_bootstrap (sadmd)
add_bootstrap (sadcd)
# add_bootstrap (sadfsd)

# add source files for "example" targets
target_sources (echod  PRIVATE ${COMMON_SOURCE_FILES})
target_sources (msg    PRIVATE ${COMMON_SOURCE_FILES})
