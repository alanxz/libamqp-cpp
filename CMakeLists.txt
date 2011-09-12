cmake_minimum_required(VERSION 2.8)

project(libamqp-cpp)

find_package(Boost 1.46.1 COMPONENTS system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(PythonInterp REQUIRED)

if (PYTHONINTERP_FOUND)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import json"
        RESULT_VARIABLE CHECK_PYTHON_JSON_FAILED
        )

    if (CHECK_PYTHON_JSON_FAILED)
        execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import simplejson"
            RESULT_VARIABLE CHECK_PYTHON_JSON_FAILED
            )
    endif (CHECK_PYTHON_JSON_FAILED)

    if (CHECK_PYTHON_JSON_FAILED)
        message(FATAL_ERROR "Could not find a python that can 'import json' or 'import simplejson'")
    endif (CHECK_PYTHON_JSON_FAILED)
endif (PYTHONINTERP_FOUND)

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

ExternalProject_Get_Property(rabbit-codegen  SOURCE_DIR)
set(AMQP_CODEGEN_DIR ${SOURCE_DIR})

enable_testing()
find_package(GTest 1.5.0)

if (GTEST_FOUND)
	option(ENABLE_TESTING "Enable unit tests" ON)
endif (GTEST_FOUND)


add_subdirectory(src)
add_subdirectory(examples)
if (ENABLE_TESTING)
	add_subdirectory(testing)
endif (ENABLE_TESTING)
