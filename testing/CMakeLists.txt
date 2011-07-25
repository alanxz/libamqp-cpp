
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)

add_executable(byteswap_test byteswap_test.cpp)
target_link_libraries(byteswap_test amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(byteswap_test byteswap_test)

add_executable(wireformat_test wireformat_test.cpp)
target_link_libraries(wireformat_test amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(wireformat_test wireformat_test)
