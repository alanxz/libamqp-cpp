#include "frame.h"

#include "wireformat.h"

#include <boost/make_shared.hpp>
#include <istream>
#include <ostream>

namespace amqpp
{
namespace detail
{

const uint8_t frame::FRAME_END = 0xCE;

frame::frame_type frame::get_frame_type(const uint8_t val)
{
  switch (val)
  {
  case frame::METHOD_TYPE:
    return frame::METHOD_TYPE;
  case frame::HEADER_TYPE:
    return frame::HEADER_TYPE;
  case frame::BODY_TYPE:
    return frame::BODY_TYPE;
  case frame::HEARTBEAT_TYPE:
    return frame::HEARTBEAT_TYPE;
  default:
    throw std::runtime_error("Invalid frame type.");
  }
}

boost::shared_ptr<frame> frame::read_frame(std::istream& i)
{
  frame_type type = frame::get_frame_type(wireformat::read_uint8(i));
  uint16_t channel = wireformat::read_uint16(i);
  uint32_t length = wireformat::read_uint32(i);

  boost::shared_ptr<frame> f = boost::make_shared<frame>(type, channel, length);
  i.read(boost::asio::buffer_cast<char*>(f->get_payload_data()), length);

  if (FRAME_END != wireformat::read_uint8(i))
  {
    throw std::runtime_error("Framing error: no 0xCE end octet");
  }
}

void frame::write_frame(const frame& f, std::ostream& o)
{
  wireformat::write_uint8(o, static_cast<uint8_t>(f.get_type()));
  wireformat::write_uint16(o, f.get_channel());
  wireformat::write_uint32(o, f.get_payload_size());

  o.write(boost::asio::buffer_cast<char*>(f.get_payload_data()), boost::asio::buffer_size(f.get_payload_data()));

  wireformat::write_uint8(o, FRAME_END);
}

frame::frame(frame_type type, uint16_t channel, const boost::asio::mutable_buffer& payload) :
  m_type(type), m_channel(channel), m_shared_buffer(), m_buffer(payload)
{
}

frame::frame(frame_type type, uint16_t channel, uint32_t payload_size) :
  m_type(type), m_channel(channel), m_shared_buffer(boost::make_shared<scoped_buffer<char> >(payload_size)), m_buffer(m_shared_buffer->get_buffer())
{
}

frame::frame(frame_type type, uint16_t channel, shared_buffer_t& shared_payload) :
  m_type(type), m_channel(channel), m_shared_buffer(shared_payload), m_buffer(m_shared_buffer->get_buffer())
{
}

frame::~frame()
{
}


} // namespace detail
} // namespace amqpp