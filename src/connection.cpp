#include "connection.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>

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
    boost::asio::io_service io_service;

    boost::asio::ip::tcp::resolver resolver(io_stream);
    boost::asio::ip::tcp::resolver::query query("localhost", "amqp");
    boost::asio::ip::tcp::resolver::iterator endpoint_it = resolver.resolve(query);

    boost::asio::ip::tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    boost::noncopyable;
    boost::asio::ip::tcp::socket sock(io_service);
    sock.get_io_service();

    boost::array<char, 8> hand_shake = {{ 'A', 'M', 'Q', 'P', 0, 0, 9, 1 }};

    socket.write_some(boost::asio::buffer(hand_shake));

    boost::array<char, 8> reply;
    socket.read_some(boost::asio::buffer(reply));

    if (reply[0] == 'A') 
    {
        // Error
        socket.close();
    }
}

} // namespace amqpp
