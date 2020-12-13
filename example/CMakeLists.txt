# example/CMakeLists.txt

# declare targets; dependencies in src/ will add
# themselves to these targets
add_executable (echod		echod.cpp)
add_executable (msg		msg.cpp)
add_executable (proto-msg 	proto-msg.cpp)
add_executable (dummy-sadmd 	dummy-sadmd.cpp)
add_executable (uuid 		uuid.cpp)
add_executable (dummy-sadcd 	dummy-sadcd.cpp)
add_executable (open-file 	open-file.cpp)
add_executable (read-file 	read-file.cpp)
add_executable (chunk-read-req  chunk-read-req.cpp)
add_executable (sadmd-tests 	sadmd-tests.cpp)
add_executable (sadcd-tests 	sadcd-tests.cpp)
add_executable (create-chunk    create-chunk.cpp)
