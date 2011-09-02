#include "connection.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

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
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

  tcp::socket socket(m_io_service);
  socket.connect(*endpoint_iterator);

  static const boost::array<char, 8> handshake = { { 'A', 'M', 'Q', 'P', 0, 0, 9, 1 } };
  boost::asio::write(socket, boost::asio::buffer(handshake));

  boost::array<char, 8> response;
  boost::asio::read(socket, boost::asio::buffer(response, 7));
  

}

} // namespace amqpp
