
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)

add_executable(test_byteswap byteswap_test.cpp)
target_link_libraries(test_byteswap amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(test_byteswap test_byteswap)

add_executable(test_wireformat wireformat_test.cpp)
target_link_libraries(test_wireformat amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(test_wireformat test_wireformat)
