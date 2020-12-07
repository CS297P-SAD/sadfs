# example/CMakeLists.txt

# declare targets; dependencies in src/ will add
# themselves to these targets
add_executable (echod       echod.cpp)
add_executable (msg         msg.cpp)
add_executable (proto-msg   proto-msg.cpp)
add_executable (dummy-sadmd dummy-sadmd.cpp)
add_executable (uuid        uuid.cpp)
add_executable (send-req        send-req.cpp)
