
set(AMQP_CODEGEN_PY "${AMQP_CODEGEN_DIR}/amqp_codegen.py")
set(CODEGEN_PY "${CMAKE_CURRENT_SOURCE_DIR}/codegen.py")
set(AMQP_SPEC_JSON_PATH "${AMQP_CODEGEN_DIR}/amqp-rabbitmq-0.9.1.json")

# Kinda hacky - we copy the amqp_codegen.py file so that our codegen.py
# can find it without having to set the PYTHONPATH
configure_file(${AMQP_CODEGEN_PY} ${CMAKE_CURRENT_SOURCE_DIR} COPYONLY)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.h
    COMMAND ${PYTHON_EXECUTABLE} ARGS ${CODEGEN_PY} header ${AMQP_SPEC_JSON_PATH} ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.h
    DEPENDS ${CODEGEN_PY} ${AMQP_SPEC_JSON_PATH} ${AMQP_CODEGEN_PY}
    VERBATIM
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.cpp
    COMMAND ${PYTHON_EXECUTABLE} ARGS ${CODEGEN_PY} body ${AMQP_SPEC_JSON_PATH} ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.cpp
    DEPENDS ${CODEGEN_PY} ${AMQP_SPEC_JSON_PATH} ${AMQP_CODEGEN_PY}
    VERBATIM
    )


include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DBOOST_ALL_NO_LIB)

set(LIBAMQP_CPP_SRCS
	byteswap.h

    connection.h
    connection.cpp

    export.h

    frame.h
    frame.cpp

    methods.h
    methods.cpp

    protocol_v091.h
    protocol_v091.cpp

    scoped_buffer.h

    table.h
    table.cpp

    wireformat.h
    wireformat.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.h
    ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.cpp

	codegen.py
	)

add_library(amqp-cpp SHARED
	${LIBAMQP_CPP_SRCS}
	)

target_link_libraries(amqp-cpp
	${Boost_LIBRARIES})
