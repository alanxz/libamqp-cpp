#ifndef _LIBAMQPCPP_CONNECTION_H_INCLUDED_
#define _LIBAMQPCPP_CONNECTION_H_INCLUDED_

#ifndef BOOST_ALL_NO_LIB
# define BOOST_ALL_NO_LIB
#endif

#include "export.h"

#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <vector>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif

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

#ifdef _MSC_VER
# pragma warning ( pop )
#endif
#endif // _LIBAMQPCPP_CONNECTION_H_INCLUDED_
