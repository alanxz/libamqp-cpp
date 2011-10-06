
set(AMQP_CODEGEN_PY "${AMQP_CODEGEN_DIR}/amqp_codegen.py")
set(CODEGEN_PY "${CMAKE_CURRENT_SOURCE_DIR}/codegen.py")
set(AMQP_SPEC_JSON_PATH "${AMQP_CODEGEN_DIR}/amqp-rabbitmq-0.9.1.json")

# Kinda hacky - we copy the amqp_codegen.py file so that our codegen.py
# can find it without having to set the PYTHONPATH
#configure_file(${AMQP_CODEGEN_PY} ${CMAKE_CURRENT_SOURCE_DIR} COPYONLY)

set(LOCAL_AMQP_CODEGEN ${CMAKE_CURRENT_SOURCE_DIR}/amqp_codegen.py)

add_custom_command(
    OUTPUT ${LOCAL_AMQP_CODEGEN}
    COMMAND ${CMAKE_COMMAND} ARGS -E copy ${AMQP_CODEGEN_PY} ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS rabbit-codegen
    VERBATIM
    )

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
if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x501)
endif (WIN32)

set(LIBAMQP_CPP_SRCS
	amqp_sasl.h
	amqp_sasl.cpp

	detail/bitset.h

	detail/byteswap.h

    channel.h

    connection.h
    connection.cpp

	detail/channel0.h
	detail/channel0.cpp

    detail/channel_impl.h
    detail/channel_impl.cpp

	detail/connection_impl.h
	detail/connection_impl.cpp

	detail/frame_builder.h
	detail/frame_builder.cpp

	detail/frame_handler.h

	detail/frame_writer.h
	detail/frame_writer.cpp

    detail/frame.h
    detail/frame.cpp

    detail/methods.h
    detail/methods.cpp

    detail/scoped_buffer.h

	detail/string_utils.h

    detail/wireformat.h
    detail/wireformat.cpp

	exception.h

    export.h

    table.h
    table.cpp

    table_entry.h
    table_entry.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.h
    ${CMAKE_CURRENT_BINARY_DIR}/methods.gen.cpp

	codegen.py
	)

add_library(amqp-cpp SHARED
	${LIBAMQP_CPP_SRCS}
	)

target_link_libraries(amqp-cpp
	${Boost_LIBRARIES})
