#include "connection_thread.h"

#include "channel_impl.h"
#include "frame.h"
#include "methods.h"
#include "methods.gen.h"

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/future.hpp>

#include <limits>
#include <stdexcept>

namespace amqpp {
namespace impl {

connection_thread::channel_future_t connection_thread::begin_open_channel()
{
  boost::promise<boost::shared_ptr<channel_impl> > promise;

  m_ioservice.post(boost::bind(&connection_thread::start_open_channel, this, promise));

  return promise.get_future();
}

void connection_impl::begin_frame_read()
{
  boost::asio::async_read(m_socket, m_builder.get_header_buffer(),
                          boost::bind(&connection_impl::on_frame_header_read, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void connection_impl::on_frame_header_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    if (m_builder.is_body_read_required())
    {
      boost::asio::async_read(m_socket, m_builder.get_body_buffer(),
                              boost::bind(&connection_impl::on_frame_body_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      frame::ptr_t received_frame = m_builder.create_frame();
      process_frame(received_frame);
      begin_frame_read();
    }
  }
  else
  {
    // TODO: Deal with the fact the read failed in some way
  }
}

void connection_impl::on_frame_body_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    frame::ptr_t received_frame = m_builder.create_frame();
    process_frame(received_frame);
    begin_frame_read();
  }
  else
  {
    // TODO: Deal with the fact the read failed in some way
  }
}

void connection_thread::dispatch_frame(const detail::frame::ptr_t& fr)
{
  uint16_t channel_id = fr->get_channel();
  if (channel_id >= m_channels.size())
  {
    throw std::runtime_error("Channel not valid!");
  }
  m_channels[chan]->process_frame(fr);
}

void connection_thread::begin_write_frame(const frame::ptr_t& fr)
{
  bool is_writing = (m_write_queue.size() > 0 ? true : false);
  m_write_queue.push(fr);

  if (!is_writing)
  {
    m_current_write_frame = m_write_queue.front();
    m_write_queue.pop();
    boost::asio::async_write(m_socket, m_writer.get_sequence(m_current_write_frame),
                              boost::bind(&connection_thread::on_write_frame, &this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
  }

}

void connection_thread::on_write_frame(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    if (m_write_queue.size() > 0)
    {
      m_current_write_frame = m_write_queue.front();
      m_write_queue.pop();
      boost::asio::async_write(m_socket, m_writer.get_sequence(m_current_write_frame),
                               boost::bind(&connection_thread::on_write_frame, &this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      m_current_write_frame.reset();
    }
  }
  else
  {
    // TODO: deal with the fact something failed
  }
}

void connection_thread::start_open_channel(channel_promise_t promise)
{
  try
  {
    channel_impl::ptr_t chan = create_next_channel();
    // Set channel promise so that it'll work
    methods::channel::open::ptr_t open = methods::channel::open::create();
    detail::frame::ptr_t frame = detail::frame::create_from_method(chan->get_channel(), open);
    begin_write_frame(frame);
  }
  catch (std::runtime_error& e)
  {
    promise.set_exception(e);
  }
}

channel_impl::ptr_t connection_thread::create_next_channel()
{
  size_t next_id = m_channels.size();
  if (next_id > std::numeric_limits<uint16_t>::max())
  {
    throw std::runtime_error("Out of channels!");
  }
  m_channels.push_back(boost::make_shared<channel_impl>(next_id));
  return m_channels.back();
}

} // namespace impl
} // namespace amqpp
