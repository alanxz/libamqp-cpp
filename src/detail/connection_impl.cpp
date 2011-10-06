#include "connection_impl.h"

#include "amqp_sasl.h"
#include "detail/channel_impl.h"
#include "exception.h"
#include "detail/frame_builder.h"
#include "detail/frame.h"
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
namespace detail
{

connection_impl::connection_impl(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost) :
  m_thread(*this)
{
  connect(host, port, username, password, vhost);
}

connection_impl::~connection_impl()
{
}


boost::shared_ptr<channel> connection_impl::open_channel()
{
  connection_manager::channel_future_t new_channel = m_thread.begin_open_channel();
  return new_channel.get();
}

void connection_impl::close()
{
}

void connection_impl::connect(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost)
{
  tcp::resolver resolver(m_thread.get_io_service());
  tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));

  boost::asio::ip::tcp::socket& sock = m_thread.get_socket();

  for (tcp::resolver::iterator it = resolver.resolve(query);
    it != tcp::resolver::iterator(); ++it)
  {
    try
    {
      sock.connect(*it);
      break;
    }
    catch (boost::system::system_error&)
    {
    }
  }
  if (!sock.is_open())
  {
    // Failed above connecting
    throw std::runtime_error("Failed to connect to remote peer");
  }

  // Send handshake
  static const boost::array<char, 8> handshake = { { 'A', 'M', 'Q', 'P', 0, 0, 9, 1 } };
  boost::asio::write(sock, boost::asio::buffer(handshake));

  method::ptr_t method = method::read(read_frame());
  methods::connection::start::ptr_t start = method_cast<methods::connection::start>(method);

  if (0 != start->get_version_major() ||
      9 != start->get_version_minor())
  {
    sock.close();
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
  frame::ptr_t fr = frame::create_from_method(0, start_ok);

  write_frame(fr);

  method = method::read(read_frame());
  methods::connection::tune::ptr_t tune = method_cast<methods::connection::tune>(method);

  methods::connection::tune_ok::ptr_t tune_ok = methods::connection::tune_ok::create();
  tune_ok->set_channel_max(tune->get_channel_max());
  tune_ok->set_frame_max(tune->get_frame_max());
  tune_ok->set_heartbeat(tune->get_heartbeat());

  fr = frame::create_from_method(0, tune_ok);
  write_frame(fr);

  methods::connection::open::ptr_t open = methods::connection::open::create();
  open->set_virtual_host(vhost);
  open->set_capabilities("");
  open->set_insist(false);
  fr = frame::create_from_method(0, open);
  write_frame(fr);

  method = method::read(read_frame());
  methods::connection::open_ok::ptr_t open_ok = method_cast<methods::connection::open_ok>(method);
  std::cout << method->to_string() << std::endl;

  // Create Thread 0?
  m_thread.start_async_read_loop();
}

void connection_impl::begin_write_method(uint16_t channel_id, const method::ptr_t& method)
{
  frame::ptr_t fr = frame::create_from_method(channel_id, method);
  m_thread.get_io_service().post(boost::bind(&connection_manager::begin_write_frame, &m_thread, fr));
}

frame::ptr_t connection_impl::read_frame()
{
  frame_builder builder;
  boost::asio::read(m_thread.get_socket(), builder.get_header_buffer());
  if (builder.is_body_read_required())
  {
    boost::asio::read(m_thread.get_socket(), builder.get_body_buffer());
  }

  return builder.create_frame();
}

void connection_impl::write_frame(const frame::ptr_t& frame)
{
  frame_writer writer;
  boost::asio::write(m_thread.get_socket(), writer.get_sequence(frame));
}


} // namespace detail
} // namespace amqpp
