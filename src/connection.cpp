#include "connection.h"
#include "frame.h"
#include "methods.gen.h"

#include <boost/array.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <istream>

using boost::asio::ip::tcp;

namespace amqpp
{

connection::connection(const std::string& host, int port, const std::string& vhost,
		const std::string& username, const std::string& password) :
	m_host(host), m_port(port), m_vhost(vhost), m_username(username), m_password(password)
{
}

connection::~connection()
{
}

void connection::connect()
{
  tcp::resolver resolver(m_io_service);
  tcp::resolver::query query("localhost", boost::lexical_cast<std::string>(5672));

  tcp::iostream io(query);
  io.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);

  static const boost::array<char, 8> handshake = { { 'A', 'M', 'Q', 'P', 0, 0, 9, 1 } };
  io.write(handshake.data(), handshake.size());

  char resp = io.peek();

  if (resp != detail::frame::METHOD_TYPE)
  {
      // connection failure
  }

  boost::shared_ptr<detail::frame> fr = detail::frame::read_frame(io);

  boost::iostreams::stream_buffer<boost::iostreams::array_source> frame_streambuf(boost::asio::buffer_cast<char*>(fr->get_payload_data()), fr->get_payload_size());

  std::istream is(&frame_streambuf);
  is.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);

  boost::shared_ptr<detail::method> method = detail::method::read(is);

  // Here we should:
  // - make sure version major/minor work
  // - make sure the broker knows our locale
  // - make sure the broker understands SASL-PLAIN
  std::cout << method->to_string();

  methods::connection::start_ok reply;
  reply.set_mechanism("PLAIN");
  reply.set_response("");
  reply.set_locale("en_US");

  // Tune/tune-ok
  // open/open-ok
}

} // namespace amqpp
