#ifndef _LIBAMQPCPP_CONNECTION_H_INCLUDED_
#define _LIBAMQPCPP_CONNECTION_H_INCLUDED_

#include <boost/noncopyable.hpp>

#include <string>

namespace amqpp 
{

class connection : boost::noncopyable
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
};
} // namespace amqpp

#endif // _LIBAMQPCPP_CONNECTION_H_INCLUDED_
