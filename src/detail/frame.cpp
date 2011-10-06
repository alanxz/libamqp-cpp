#include "frame.h"

#include "methods.gen.h"
#include "detail/wireformat.h"

#include <boost/array.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/make_shared.hpp>

#include <istream>
#include <ostream>
#include <stdexcept>

namespace amqpp
{
namespace detail
{

typedef boost::iostreams::stream<boost::iostreams::array_sink> array_ostream;
typedef boost::iostreams::stream<boost::iostreams::array_source> array_istream;

const uint8_t frame::FRAME_END = detail::FRAME_END;

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

frame::ptr_t frame::create_from_method(uint16_t channel, const detail::method::ptr_t method)
{
  frame::ptr_t fr = boost::make_shared<frame>(METHOD_TYPE, channel, method->get_serialized_size());

  array_ostream os(fr->m_shared_buffer->get_data(), fr->m_shared_buffer->get_size());

  method->write(os);

  return fr;
}

frame::frame(frame_type type, uint16_t channel, const shared_buffer_t& buffer):
m_type(type), m_channel(channel), m_shared_buffer(buffer)
{
}

frame::frame(frame_type type, uint16_t channel, uint32_t payload_size) :
m_type(type), m_channel(channel), m_shared_buffer(boost::make_shared<scoped_buffer<char> >(payload_size))
{
}

frame::~frame()
{
}


} // namespace detail
} // namespace amqpp
