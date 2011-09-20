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
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace amqpp
{
namespace detail
{
class frame;
}
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
  virtual void connect(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost);


private:
  boost::shared_ptr<detail::frame> read_frame();
  void write_frame(const boost::shared_ptr<detail::frame>& frame);

  boost::asio::io_service m_ioservice;
  boost::asio::ip::tcp::socket m_socket;

  detail::frame_builder m_framebuilder;
  detail::frame_writer m_framewriter;

};

} // namespace impl
} // namespace amqpp
#endif // _LIBAMQPP_CONNECTION_IMPL_H_INCLUDED_