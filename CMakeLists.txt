cmake_minimum_required(VERSION 2.8)

project(libamqp-cpp)

find_package(Boost 1.46.1 COMPONENTS system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include(ExternalProject)
ExternalProject_Add(
	rabbit-codegen
	PREFIX rabbitmq-codegen
	GIT_REPOSITORY https://github.com/rabbitmq/rabbitmq-codegen
	GIT_TAG amqp_0_9_1
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	TEST_COMMAND ""
	)

enable_testing()
find_package(GTest 1.5.0)

if (GTEST_FOUND)
	option(ENABLE_TESTING "Enable unit tests" ON)
endif (GTEST_FOUND)


add_subdirectory(src)
if (ENABLE_TESTING)
	add_subdirectory(testing)
endif (ENABLE_TESTING)
