#ifndef _LIBAMQPP_CONNECTION_MANAGER_H_INCLUDED_H_
#define _LIBAMQPP_CONNECTION_MANAGER_H_INCLUDED_H_

#include "detail/channel0.h"
#include "detail/channel_impl.h"
#include "detail/frame.h"
#include "detail/frame_builder.h"
#include "detail/frame_handler.h"
#include "detail/frame_writer.h"

#ifndef BOOST_ALL_NO_LIB
# define BOOST_ALL_NO_LIB
#endif

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>

#include <queue>
#include <vector>

namespace amqpp
{
namespace detail
{

class channel_impl;
class connection_impl;

class connection_manager : boost::noncopyable
{
public: // Called from other threads
  explicit connection_manager(connection_impl& impl);
  virtual ~connection_manager();

  inline boost::asio::io_service& get_io_service() { return m_ioservice; }
  inline boost::asio::ip::tcp::socket& get_socket() { return m_socket; }

  void start_async_read_loop();

  typedef boost::promise<boost::shared_ptr<channel_impl> > channel_promise_t;
  typedef boost::shared_ptr<channel_promise_t> channel_promise_ptr_t;
  typedef boost::unique_future<boost::shared_ptr<channel_impl> > channel_future_t;

  channel_future_t begin_open_channel();

public: // Stuff that is only ever called from within the io_service thread

  void begin_frame_read();
  void on_frame_header_read(const boost::system::error_code& ec, size_t bytes_transferred);
  void on_frame_body_read(const boost::system::error_code& ec, size_t bytes_transferred);
  void dispatch_frame(const frame::ptr_t& fr);

  void begin_write_frame(const frame::ptr_t& fr);
  void on_write_frame(const boost::system::error_code& ec, size_t bytes_transferred);


  void start_open_channel(channel_promise_ptr_t channel_promise);
  boost::shared_ptr<channel_impl> create_next_channel(const channel_promise_ptr_t& promise);


private:
  boost::asio::io_service m_ioservice;
  boost::asio::ip::tcp::socket m_socket;
  std::vector<boost::weak_ptr<frame_handler> > m_channels;
  boost::shared_ptr<channel0> m_channel0;
  std::queue<frame::ptr_t> m_write_queue;

  frame_builder m_builder;
  frame_writer m_writer;
  frame::ptr_t m_current_write_frame;
  connection_impl& m_connection;

};
} // namespace detail
} // namespace amqpp
#endif // _LIBAMQPP_CONNECTION_MANAGER_H_INCLUDED_H_
