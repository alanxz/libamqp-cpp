#ifndef _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_
#define _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_

#include "connection.h"
#include "detail/connection_manager.h"
#include "detail/frame.h"


#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/cstdint.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>

#include <string>

namespace amqpp
{
namespace detail
{

class frame;
class channel_impl;

class connection_impl : public amqpp::connection, public boost::noncopyable, public boost::enable_shared_from_this<connection_impl>
{
public:
  explicit connection_impl(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost);
  virtual ~connection_impl();

  virtual boost::shared_ptr<channel> open_channel();
  virtual void close();

public:
  // Internal interface
  void connect(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost);

  void begin_write_method(uint16_t channel_id, const method::ptr_t& method);

private:


  connection_manager m_thread;
};

} // namespace detail
} // namespace amqpp

#endif // _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_
