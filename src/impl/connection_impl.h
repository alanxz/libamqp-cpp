#ifndef _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_
#define _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_

#include "connection.h"
#include "frame_builder.h"
#include "frame_writer.h"

#ifndef BOOST_ALL_NO_LIB
# define BOOST_ALL_NO_LIB
#endif

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/cstdint.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>

#include <string>
#include <vector>

namespace amqpp
{
namespace detail
{
class frame;
}
namespace impl
{

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
  virtual void connect(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost);


  void process_frame(detail::frame::ptr_t& frame);
  void begin_async_frame_read();

  typedef boost::unique_future<boost::shared_ptr<channel> > channel_future_t;
  channel_future_t begin_open_channel();

  uint16_t get_next_channel_id();

private:
  boost::shared_ptr<detail::frame> read_frame();
  void write_frame(const boost::shared_ptr<detail::frame>& frame);

  boost::asio::io_service m_ioservice;
  boost::asio::ip::tcp::socket m_socket;

};

} // namespace impl
} // namespace amqpp
#endif // _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_
