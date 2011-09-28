#ifndef AMQPP_IMPL_CONNECTION_THREAD_H
#define AMQPP_IMPL_CONNECTION_THREAD_H

#include "channel_impl.h"
#include "frame.h"
#include "frame_builder.h"
#include "frame_writer.h"


#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>

#include <vector>
#include <queue>

namespace amqpp {
namespace impl {

class channel_impl;

class connection_thread : boost::noncopyable
{
public: // Called from other threads
  typedef boost::unique_future<channel_impl::ptr_t> channel_future_t;
  channel_future_t begin_open_channel();


public: // Stuff that is only ever called from within the io_service thread

  void begin_frame_read();
  void on_frame_header_read(const boost::system::error_code& ec, size_t bytes_transferred);
  void on_frame_body_read(const boost::system::error_code& ec, size_t bytes_transferred);
  void dispatch_frame(const frame::ptr_t& fr);

  void begin_write_frame(const frame::ptr_t& fr);
  void on_write_frame(const boost::system::error_code& ec, size_t bytes_transferred);

  typedef boost::promise<channel_impl::ptr_t> channel_promise_t;
  void start_open_channel(channel_promise_t channel_promise);
  channel_impl::ptr_t create_next_channel();


private:
  boost::asio::io_service m_ioservice;
  boost::asio::ip::tcp::socket m_socket;
  std::vector<boost::shared_ptr<channel_impl> > m_channels;
  std::queue<frame::ptr_t> m_write_queue;

  detail::frame_builder m_builder;
  detail::frame_writer m_writer;
  detail::frame::ptr_t m_current_write_frame;

};

} // namespace impl
} // namespace amqpp

#endif // AMQPP_IMPL_CONNECTION_THREAD_H
