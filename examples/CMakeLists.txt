
include_directories(../src)
include_directories(${CMAKE_BINARY_DIR}/src)

add_executable(example_connect connect_example.cpp)
target_link_libraries(example_connect amqp-cpp)
