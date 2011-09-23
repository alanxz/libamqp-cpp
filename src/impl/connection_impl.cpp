#include "connection_impl.h"

#include "amqp_sasl.h"
#include "channel_impl.h"
#include "frame_builder.h"
#include "frame.h"
#include "methods.gen.h"
#include "table.h"
#include "table_entry.h"

#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/system/system_error.hpp>

#include <algorithm>
#include <istream>
#include <limits>
#include <ostream>

using boost::asio::ip::tcp;

namespace amqpp
{
namespace impl
{

connection_impl::connection_impl(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost):
  m_ioservice(), m_socket(m_ioservice)
{
  connect(host, port, username, password, vhost);
}

connection_impl::~connection_impl()
{
}

uint16_t connection_impl::get_next_channel_id()
{
  size_t next_id = m_channelmap.size();
  if (next_id > std::numeric_limits<uint16_t>::max())
  {
    throw std::runtime_error("Out of channels!");
  }
  m_channelmap.push_back(boost::make_shared<channel_impl>(next_id, this->shared_from_this()));
}

boost::shared_ptr<channel> connection_impl::open_channel()
{
  uint16_t channel_id = get_next_channel_id();

  methods::channel::open::ptr_t open = methods::channel::open::create();
  detail::frame::ptr_t fr = detail::frame::create_from_method(channel_id, open);

  channel_impl::ptr_t channel = m_channelmap[channel_id];
  channel->rpc_reply = channel->rpc_promise.get_future();

  begin_async_write_frame(fr);
  fr = channel->rpc_reply->get();
  return channel;
}

void connection_impl::begin_async_write_frame(detail::frame::ptr_t& fr)
{
  m_framewriter.get_sequence(fr);
  boost::asio::async_write(m_socket, m_framewriter, boost::bind(&connection_impl::on_frame_write, this,
                                                                boost::asio::placeholder::error,
                                                                boost::asio::placeholder::bytes_transferred));
}

void connection_impl::on_frame_write(const boost::system::error_code& ec, size_t bytes_transferred)
{
}

void connection_impl::close()
{
}

void connection_impl::connect(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost)
{
  tcp::resolver resolver(m_ioservice);
  tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));

  for (tcp::resolver::iterator it = resolver.resolve(query);
    it != tcp::resolver::iterator(); ++it)
  {
    try
    {
      m_socket.connect(*it);
      break;
    }
    catch (boost::system::system_error&)
    {
    }
  }
  if (!m_socket.is_open())
  {
    // Failed above connecting
    throw std::runtime_error("Failed to connect to remote peer");
  }

  // Send handshake
  static const boost::array<char, 8> handshake = { { 'A', 'M', 'Q', 'P', 0, 0, 9, 1 } };
  boost::asio::write(m_socket, boost::asio::buffer(handshake));

  detail::method::ptr_t method = detail::method::read(read_frame());
  methods::connection::start::ptr_t start = detail::method_cast<methods::connection::start>(method);

  if (0 != start->get_version_major() ||
      9 != start->get_version_minor())
  {
    m_socket.close();
    throw std::runtime_error("Broker is using the wrong version of AMQP");
  }

  std::cout << start->to_string();
  std::cout << std::endl;

  methods::connection::start_ok::ptr_t start_ok = methods::connection::start_ok::create();
  start_ok->get_client_properties().insert(table_entry("product", std::string("libamqp-cpp")));
  start_ok->get_client_properties().insert(table_entry("version", std::string("0.1b")));
  start_ok->get_client_properties().insert(table_entry("platform", std::string("c++")));
  start_ok->get_client_properties().insert(table_entry("copyright", std::string("Alan Antonuk (c) 2011")));
  start_ok->get_client_properties().insert(table_entry("information", std::string("http://github.com/alanxz/libamqp-cpp")));

  std::string mechanism = sasl::select_sasl_mechanism(start->get_mechanisms());
  start_ok->set_mechanism(mechanism);
  start_ok->set_response(sasl::get_sasl_response(mechanism, username, password));
  start_ok->set_locale("en_US");

  std::cout << start_ok->to_string() << std::endl;
  detail::frame::ptr_t fr = detail::frame::create_from_method(0, start_ok);

  write_frame(fr);

  method = detail::method::read(read_frame());
  methods::connection::tune::ptr_t tune = detail::method_cast<methods::connection::tune>(method);

  methods::connection::tune_ok::ptr_t tune_ok = methods::connection::tune_ok::create();
  tune_ok->set_channel_max(tune->get_channel_max());
  tune_ok->set_frame_max(tune->get_frame_max());
  tune_ok->set_heartbeat(tune->get_heartbeat());

  fr = detail::frame::create_from_method(0, tune_ok);
  write_frame(fr);

  methods::connection::open::ptr_t open = methods::connection::open::create();
  open->set_virtual_host(vhost);
  open->set_capabilities("");
  open->set_insist(false);
  fr = detail::frame::create_from_method(0, open);
  write_frame(fr);

  method = detail::method::read(read_frame());
  methods::connection::open_ok::ptr_t open_ok = detail::method_cast<methods::connection::open_ok>(method);
  std::cout << method->to_string() << std::endl;

  m_channelmap.push_back(boost::make_shared<channel_impl>(0, this->shared_from_this()));

  begin_async_frame_read();
  boost::thread connection_thread(boost::bind(&boost::asio::io_service::run, &m_ioservice));
}

void connection_impl::begin_async_frame_read()
{
  m_framebuilder.reset();
  boost::asio::async_read(m_socket, m_framebuilder.get_header_buffer(),
                          boost::bind(&connection_impl::on_frame_header_read, this,
                                      boost::asio::placeholder::error,
                                      boost::asio::placeholder::bytes_transferred));
}

void connection_impl::on_frame_header_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    if (m_framebuilder.is_body_read_required())
    {
      boost::asio::async_read(m_socket, m_framebuilder.get_body_buffer(),
                              boost::bind(&connection_impl::on_frame_body_read, this,
                                          boost::asio::placeholder::error,
                                          boost::asio::placeholder::bytes_transferred));
    }
    else
    {
      process_frame(m_framebuilder.create_frame());
      begin_async_frame_read();
    }
  }
  else
  {
    // Shit is going down

  }
}

void connection_impl::on_frame_body_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
  if (!ec)
  {
    process_frame(m_framebuilder.create_frame());
    begin_async_frame_read();
  }
  else
  {
    // Shit is going down
  }
}

void connection_impl::process_frame(detail::frame::ptr_t& fr)
{
  channel_impl::ptr_t channel = m_channelmap[fr->get_channel()];
  channel->rpc_promise.set_value(fr);
}

void connection_impl::close()
{
  methods::connection::close::ptr_t close = methods::connection::close::create();
  fr = detail::frame::create_from_method(0, close);
  write_frame(fr);

  method = detail::method::read(read_frame());
  methods::connection::close_ok::ptr_t close_ok = detail::method_cast<methods::connection::close_ok>(method);
  std::cout << method->to_string() << std::endl;
}

detail::frame::ptr_t connection_impl::read_frame()
{
  m_framebuilder.reset();
  boost::asio::read(m_socket, m_framebuilder.get_header_buffer());
  if (m_framebuilder.is_body_read_required())
  {
    boost::asio::read(m_socket, m_framebuilder.get_body_buffer());
  }

  return m_framebuilder.create_frame();
}

void connection_impl::write_frame(const detail::frame::ptr_t& frame)
{
  boost::asio::write(m_socket, m_framewriter.get_sequence(frame));
}
} // namespace impl
} // namespace amqpp
