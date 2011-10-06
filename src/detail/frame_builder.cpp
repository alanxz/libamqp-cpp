#include "frame_builder.h"
#include "detail/wireformat.h"

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include <stdexcept>

namespace amqpp
{
namespace detail
{

typedef boost::iostreams::stream<boost::iostreams::array_source> array_istream;

frame_builder::frame_builder()
{
  reset();
}

frame_builder::~frame_builder()
{
}

void frame_builder::reset()
{
  m_state = invalid;
  m_header.assign(0);
  m_channel = 0;
  m_body = boost::shared_ptr<scoped_buffer<char> >();
  m_end.assign(0);
}

frame_builder::buffer_t frame_builder::get_header_buffer()
{
  return boost::asio::buffer(m_header);
}

bool frame_builder::is_body_read_required()
{
  validate_header();

  if (m_body->get_size() == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

frame_builder::buffer_sequence_t frame_builder::get_body_buffer()
{
  validate_header();
  buffer_sequence_t sequence = { { boost::asio::buffer(m_body->get_data() + 1, m_body->get_size() - 1), boost::asio::buffer(m_end) } };
  return sequence;
}

frame::ptr_t frame_builder::create_frame()
{
  validate_body();
  return boost::make_shared<frame>(m_type, m_channel, m_body);
}

void frame_builder::validate_header()
{
  if (m_state == invalid)
  {
    array_istream is(reinterpret_cast<char*>(m_header.data()), m_header.size());
    m_type = frame::get_frame_type(wireformat::read_uint8(is));
    m_channel = wireformat::read_uint16(is);
    uint32_t size = wireformat::read_uint32(is);
    uint8_t last = wireformat::read_uint8(is);

    if (size == 0)
    {
      if (last != frame::FRAME_END)
      {
        throw std::runtime_error("Framing error: bad end octet");
      }
      m_state = valid_both;
      m_body = boost::make_shared<scoped_buffer<char> >(0);
    }
    else
    {
      m_state = valid_header;
      m_body = boost::make_shared<scoped_buffer<char> >(size);
      m_body->get_data()[0] = last;
    }
  }
}

void frame_builder::validate_body()
{
  switch (m_state)
  {
  case invalid:
    throw std::logic_error("validate_body called before a valid header existed");
  case valid_header:
  {
    if (m_end[0] != frame::FRAME_END)
    {
      throw std::runtime_error("Framing error: bad end octet");
    }
    break;
  }
  case valid_both:
    break;
  default:
    throw std::logic_error("unknown builder state");
  }
}

}
} // namespace amqpp
