#ifndef _LIBAMQPCPP_CONNECTION_H_INCLUDED_
#define _LIBAMQPCPP_CONNECTION_H_INCLUDED_

#define BOOST_ALL_NO_LIB

#include "export.h"

#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>

#include <string>

namespace amqpp 
{

class AMQPP_EXPORT connection : boost::noncopyable
{
	public:
		explicit connection(const std::string& host = "localhost",
				int port = 5672,
				const std::string& vhost  = "/",
				const std::string& username = "guest",
				const std::string& password = "guest");
        virtual ~connection();

        void connect();

	private:
		const std::string m_host;
		const int m_port;
		const std::string m_vhost;
		const std::string m_username;
		const std::string m_password;

    boost::asio::io_service m_io_service;
};
} // namespace amqpp

#endif // _LIBAMQPCPP_CONNECTION_H_INCLUDED_
