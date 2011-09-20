#ifndef _LIBAMQPCPP_CONNECTION_H_INCLUDED_
#define _LIBAMQPCPP_CONNECTION_H_INCLUDED_

#include "export.h"

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace amqpp 
{

class channel;

class AMQPP_EXPORT connection
{
	public:

    static boost::shared_ptr<connection> create_connection(const std::string& host, 
      const std::string username = "guest", const std::string& password = "guest", 
      const std::string& vhost = "/", uint16_t port = 5672);

    virtual boost::shared_ptr<channel> open_channel() = 0;
    virtual void close() = 0;
};
} // namespace amqpp

#endif // _LIBAMQPCPP_CONNECTION_H_INCLUDED_
