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

	wireformat.h
	wireformat.cpp
	)

add_library(amqp-cpp SHARED
	${LIBAMQP_CPP_SRCS}
	)

target_link_libraries(amqp-cpp
	${Boost_LIBRARIES})
