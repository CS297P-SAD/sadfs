# src/bootstrap/CMakeLists.txt
# define a function to do the following for every main daemon target
# 1. add a corresponding bootstrap target
# 2. mark it as a dependency of the main target
# 3. inform CMake of any external dependencies
function (add_bootstrap TARGET)
	set (BOOTSTRAP_TARGET ${TARGET}-bootstrap)
	add_executable (${BOOTSTRAP_TARGET}
		${BOOTSTRAP_TARGET}.cpp util.cpp)
	add_dependencies (${TARGET} ${BOOTSTRAP_TARGET})
	target_include_directories (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_INCLUDE_DIRS})
	target_link_libraries (${BOOTSTRAP_TARGET}
		PRIVATE ${Boost_LIBRARIES})
endfunction()

# first, find the boost_program_options library. this sets
# the Boost_INCLUDE_DIRS and Boost_LIBRARIES variables
find_package (Boost
	1.67.0
	REQUIRED
	COMPONENTS program_options)

add_bootstrap (sadmd)
# add_bootstrap (sadcd)
# add_bootstrap (sadfsd)
