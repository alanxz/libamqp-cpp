cmake_minimum_required(VERSION 2.8)

project(libamqp-cpp)

find_package(Boost 1.46.1 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

enable_testing()
find_package(GTest 1.5.0)

if (GTEST_FOUND)
	option(ENABLE_TESTING "Enable unit tests" ON)
endif (GTEST_FOUND)


add_subdirectory(src)
if (ENABLE_TESTING)
	add_subdirectory(testing)
endif (ENABLE_TESTING)
