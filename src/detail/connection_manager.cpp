#include "detail/connection_impl.h"
#include "connection_manager.h"

#include "exception.h"

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/thread/thread.hpp>

namespace amqpp
{
namespace detail
{

connection_manager::connection_manager(connection_impl& connection_imp) :
  m_socket(m_ioservice), m_connection(connection_imp)
{
}

connection_manager::~connection_manager()
{
}

void connection_manager::start_async_read_loop()
{
  m_channel0 = boost::make_shared<channel0>();
  m_channels.push_back(boost::weak_ptr<frame_handler>(m_channel0));

  begin_frame_read();
  boost::thread conn_thread(boost::bind(&boost::asio::io_service::run, &m_ioservice));
}

connection_manager::channel_future_t connection_manager::begin_open_channel()
{
  channel_promise_ptr_t promise = boost::make_shared<channel_promise_t>();

  m_ioservice.post(boost::bind(&connection_manager::start_open_channel, this, promise));

  return promise->get_future();
}

void connection_manager::begin_frame_read()
{
  boost::asio::async_read(m_socket, m_builder.get_header_buffer(),
                          boost::bind(&connection_manager::on_frame_header_read, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void connection_manager::on_frame_header_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    if (m_builder.is_body_read_required())
    {
      boost::asio::async_read(m_socket, m_builder.get_body_buffer(),
                              boost::bind(&connection_manager::on_frame_body_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      frame::ptr_t received_frame = m_builder.create_frame();
      dispatch_frame(received_frame);
      begin_frame_read();
    }
  }
  else
  {
    // TODO: Deal with the fact the read failed in some way
  }
}

void connection_manager::on_frame_body_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    frame::ptr_t received_frame = m_builder.create_frame();
    dispatch_frame(received_frame);
    begin_frame_read();
  }
  else
  {
    // TODO: Deal with the fact the read failed in some way
  }
}

void connection_manager::dispatch_frame(const frame::ptr_t& fr)
{
  uint16_t channel_id = fr->get_channel();
  if (channel_id >= m_channels.size())
  {
    throw std::runtime_error("Channel not valid!");
  }
  boost::shared_ptr<frame_handler> fh = m_channels[channel_id].lock();
  if (fh == boost::shared_ptr<frame_handler>())
  {
    // channel has been destructed....
    throw std::runtime_error("Channel doens't exist!");
  }
  try
  {
    fh->process_frame(fr);
  }
  catch (amqpp::connection_exception&)
  {
    // Need to start shutting down the connection
  }
}

void connection_manager::begin_write_frame(const frame::ptr_t& fr)
{
  bool is_writing = (m_write_queue.size() > 0 ? true : false);
  m_write_queue.push(fr);

  if (!is_writing)
  {
    m_current_write_frame = m_write_queue.front();
    m_write_queue.pop();
    boost::asio::async_write(m_socket, m_writer.get_sequence(m_current_write_frame),
                              boost::bind(&connection_manager::on_write_frame, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
  }

}

void connection_manager::on_write_frame(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    if (m_write_queue.size() > 0)
    {
      m_current_write_frame = m_write_queue.front();
      m_write_queue.pop();
      boost::asio::async_write(m_socket, m_writer.get_sequence(m_current_write_frame),
                               boost::bind(&connection_manager::on_write_frame, this,
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

void connection_manager::start_open_channel(channel_promise_ptr_t promise)
{
  try
  {
    channel_impl::ptr_t chan = create_next_channel(promise);
    methods::channel::open::ptr_t open = methods::channel::open::create();
    frame::ptr_t frame = frame::create_from_method(chan->get_channel_id(), open);
    begin_write_frame(frame);
  }
  catch (std::runtime_error& e)
  {
    promise->set_exception(boost::copy_exception(e));
  }
}

channel_impl::ptr_t connection_manager::create_next_channel(const channel_promise_ptr_t& promise)
{
  size_t next_id = m_channels.size();
  if (next_id > std::numeric_limits<uint16_t>::max())
  {
    throw std::runtime_error("Out of channels!");
  }
  channel_impl::ptr_t new_channel = boost::make_shared<channel_impl>(static_cast<uint16_t>(next_id), m_connection.shared_from_this(), promise);
  m_channels.push_back(new_channel);
  return new_channel;
}

frame::ptr_t connection_manager::read_frame()
{
  boost::asio::read(m_socket, m_builder.get_header_buffer());
  if (m_builder.is_body_read_required())
  {
    boost::asio::read(m_socket, m_builder.get_body_buffer());
  }

  return m_builder.create_frame();
}

void connection_manager::write_frame(const frame::ptr_t& frame)
{
  boost::asio::write(m_socket, m_writer.get_sequence(frame));
}

} // namespace detail
} // namespace amqpp
