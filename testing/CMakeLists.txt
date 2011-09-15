
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)

add_executable(test_byteswap byteswap_test.cpp)
target_link_libraries(test_byteswap amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(test_byteswap test_byteswap)

add_executable(test_wireformat wireformat_test.cpp)
target_link_libraries(test_wireformat amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(test_wireformat test_wireformat)

add_executable(test_table table_test.cpp)
target_link_libraries(test_table amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(test_table test_table)

add_executable(test_method method_test.cpp)
target_link_libraries(test_method amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(test_method test_method)

add_executable(test_frame frame_test.cpp)
target_link_libraries(test_frame amqp-cpp ${GTEST_BOTH_LIBRARIES})
add_test(test_frame test_frame)
