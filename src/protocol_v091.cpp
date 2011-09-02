#include "protocol_v091.h"

#include "wireformat.h"

#include <boost/array.hpp>

namespace amqpp
{


const uint8_t protocol_v091::FRAME_END = 0xCE;

protocol_v091::protocol_v091()
{
}

protocol_v091::~protocol_v091()
{
}

void protocol_v091::handshake(std::iostream& io)
{
  static const boost::array<char, 8> handshake = {{'A', 'M', 'Q', 'P', 0, 0, 9, 1}};

  io.write(handshake.data(), handshake.size());
  io.flush();

  
}

protocol_v091::frame protocol_v091::read_frame(std::istream& is)
{
  frame fr;

  uint8_t encoded_frame_type = detail::wireformat::read_uint8(is);
  fr.frame_type = get_frame_type(encoded_frame_type);

  fr.channel = detail::wireformat::read_uint16(is);
  fr.size = detail::wireformat::read_uint32(is);

  is.read(boost::asio::buffer_cast<char*>(fr.payload), boost::asio::buffer_size(fr.payload));

  uint8_t end = detail::wireformat::read_uint8(is);
  if (end != FRAME_END)
  {
    throw std::runtime_error("Framing error: no 0xCE frame ending");
  }

}

void protocol_v091::write_frame(std::ostream& os, const protocol_v091::frame& frame)
{
  detail::wireformat::write_uint8(os, static_cast<uint8_t>(frame.frame_type));
  detail::wireformat::write_uint16(os, frame.channel);
  detail::wireformat::write_uint32(os, frame.size);
  os.write(boost::asio::buffer_cast<const char*>(frame.payload), boost::asio::buffer_size(frame.payload));
  detail::wireformat::write_uint8(os, FRAME_END);
}

protocol_v091::frame protocol_v091::read_frame_header(std::istream& is)
{
  uint8_t type = detail::wireformat::read_uint8(is);
  uint16_t channel = detail::wireformat::read_uint16(is);
  uint32_t size = detail::wireformat::read_uint32(is);

  frame ret = { size, channel, get_frame_type(type) };

  return ret;
}

void protocol_v091::validate_frame(std::istream& is, protocol_v091::frame& current_frame)
{
  is.seekg(current_frame.size, std::ios_base::cur);
  
  uint8_t end_octet = detail::wireformat::read_uint8(is);

  if (end_octet != 0xCE)
  {
    throw std::runtime_error("Framing error");
  }
}

protocol_v091::frame_t protocol_v091::get_frame_type(uint8_t type)
{
  switch (type)
  {
  case protocol_v091::method_frame:
    return method_frame;
  case protocol_v091::header_frame:
    return header_frame;
  case protocol_v091::body_frame:
    return body_frame;
  case protocol_v091::heartbeat_frame:
    return heartbeat_frame;
  default:
    throw std::runtime_error("Unrecognized frame type.");
  }
}

} // namespace amqpp