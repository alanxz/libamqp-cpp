#include "connection.h"

#include "impl/connection_impl.h"

#include <boost/make_shared.hpp>

namespace amqpp
{

boost::shared_ptr<connection> connection::create_connection(const std::string& host, 
      const std::string username, const std::string& password,
      const std::string& vhost, uint16_t port)
{
  return boost::make_shared<impl::connection_impl>(host, port, username, password, vhost);
}

} // namespace amqpp
