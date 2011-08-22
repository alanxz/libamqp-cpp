set(LIBAMQP_CPP_SRCS
	byteswap.h

	wireformat.h
	wireformat.cpp

    connection.h
    connection.cpp
	)

add_library(amqp-cpp SHARED
	${LIBAMQP_CPP_SRCS}
	)
