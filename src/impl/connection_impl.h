#ifndef _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_
#define _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_

#include "connection.h"

#ifndef BOOST_ALL_NO_LIB
# define BOOST_ALL_NO_LIB
#endif

#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>

#include <string>

namespace amqpp
{
namespace impl
{

class connection_impl : public amqpp::connection, boost::noncopyable
{
public:
  explicit connection_impl(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost);
  virtual ~connection_impl();

  virtual boost::shared_ptr<channel> open_channel();
  virtual void close();

public:
  // Internal interface
  virtual void connect();


private:
  boost::asio::io_service m_ioservice;

};

} // namespace impl
} // namespace amqpp
#endif // _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_