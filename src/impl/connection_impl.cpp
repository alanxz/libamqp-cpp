#include "connection_impl.h"

#include "amqp_sasl.h"
#include "frame.h"
#include "methods.gen.h"
#include "table.h"
#include "table_entry.h"

#include <boost/array.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <algorithm>
#include <istream>
#include <ostream>

using boost::asio::ip::tcp;

namespace amqpp
{
namespace impl
{

connection_impl::connection_impl(const std::string& host, uint16_t port, const std::string& username, const std::string& password, const std::string& vhost)
{
}

connection_impl::~connection_impl()
{
}

boost::shared_ptr<channel> connection_impl::open_channel()
{
  return boost::shared_ptr<channel>();
}

void connection_impl::close()
{
}

void connection_impl::connect()
{
  tcp::resolver resolver(m_ioservice);
  tcp::resolver::query query("localhost", boost::lexical_cast<std::string>(5672));

  tcp::iostream io(query);
  io.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);

  static const boost::array<char, 8> handshake = { { 'A', 'M', 'Q', 'P', 0, 0, 9, 1 } };
  io.write(handshake.data(), handshake.size());
  io.flush();

  char resp = io.peek();

  if (resp != detail::frame::METHOD_TYPE)
  {
      // connection failure
  }

  detail::frame::ptr_t fr = detail::frame::read_frame(io);
  detail::method::ptr_t method = detail::method::read(fr);
  methods::connection::start::ptr_t start_method = detail::method_cast<methods::connection::start>(method);

  if (0 != start_method->get_version_major() ||
      9 != start_method->get_version_minor())
  {
    // fail
  }

  std::cout << start_method->to_string();
  std::cout << std::endl;

  methods::connection::start_ok::ptr_t start_ok = methods::connection::start_ok::create();
  start_ok->get_client_properties().insert(table_entry("product", "libamqp-cpp"));
  start_ok->get_client_properties().insert(table_entry("version", "0.1b"));
  start_ok->get_client_properties().insert(table_entry("platform", "c++"));
  start_ok->get_client_properties().insert(table_entry("copyright", "Alan Antonuk (c) 2011"));
  start_ok->get_client_properties().insert(table_entry("information", "http://github.com/alanxz/libamqp-cpp"));

  std::string mechanism = sasl::select_sasl_mechanism(start_method->get_mechanisms());
  start_ok->set_mechanism(mechanism);
  start_ok->set_response(sasl::get_sasl_response(mechanism, m_username, m_password));
  start_ok->set_locale("en_US");

  std::cout << start_ok->to_string() << std::endl;
  fr = detail::frame::create_from_method(0, start_ok);

  fr->write(io);

  fr = detail::frame::read_frame(io);
  method = detail::method::read(fr);
  std::cout << method->to_string() << std::endl;

  methods::connection::tune::ptr_t tune = detail::method_cast<methods::connection::tune>(method);

  methods::connection::tune_ok::ptr_t tune_ok = methods::connection::tune_ok::create();
  tune_ok->set_channel_max(tune->get_channel_max());
  tune_ok->set_frame_max(tune->get_frame_max());
  tune_ok->set_heartbeat(tune->get_heartbeat());

  std::cout << tune_ok->to_string() << std::endl;
  fr = detail::frame::create_from_method(0, tune_ok);
  fr->write(io);


  methods::connection::open::ptr_t open = methods::connection::open::create();
  open->set_virtual_host("/");
  open->set_capabilities("");
  open->set_insist(false);
  fr = detail::frame::create_from_method(0, open);
  fr->write(io);

  fr = detail::frame::read_frame(io);
  method = detail::method::read(fr);
  std::cout << method->to_string() << std::endl;

  methods::connection::close::ptr_t close = methods::connection::close::create();
  fr = detail::frame::create_from_method(0, close);
  fr->write(io);
  fr = detail::frame::read_frame(io);
  method = detail::method::read(fr);
  std::cout << method->to_string() << std::endl;

}
} // namespace impl
} // namespace amqpp