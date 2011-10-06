#include "frame_writer.h"

#include "detail/frame.h"
#include "detail/wireformat.h"

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio/buffer.hpp>

namespace amqpp
{
namespace detail
{

const boost::array<uint8_t, 1> frame_writer::FOOTER = { { frame::FRAME_END } };

typedef boost::iostreams::stream<boost::iostreams::array_sink> array_ostream;

frame_writer::buffer_sequence_t frame_writer::get_sequence(const frame::ptr_t& frame)
{
  array_ostream os(reinterpret_cast<char*>(m_header.data()), m_header.size());

  wireformat::write_uint8(os, static_cast<uint8_t>(frame->get_type()));
  wireformat::write_uint16(os, frame->get_channel());
  wireformat::write_uint32(os, frame->get_shared_buffer()->get_size());

  char* ptr = frame->get_shared_buffer()->get_data();
  size_t val = frame->get_shared_buffer()->get_size();

  buffer_sequence_t ret = {{ boost::asio::buffer(m_header.data(), m_header.size()),
                            boost::asio::buffer(ptr, val),
                            boost::asio::buffer(FOOTER.data(), FOOTER.size()) }};

  for (int i = 0; i < 3; ++i)
  {
    size_t val2 = boost::asio::buffer_size(ret[i]);
    const char* ptr2 = boost::asio::buffer_cast<const char*>(ret[i]);
  }
    return ret;
}

} // namespace detail
} // namespace amqp
