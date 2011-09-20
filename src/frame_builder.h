#ifndef _LIBAMQPP_FRAME_BUILDER_H_INCLUDED_
#define _LIBAMQPP_FRAME_BUILDER_H_INCLUDED_

#include "frame.h"

#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/noncopyable.hpp>

namespace amqpp
{
namespace detail
{

class frame_builder : boost::noncopyable
{
public:
  typedef boost::asio::mutable_buffers_1 buffer_t;
  typedef boost::array<boost::asio::mutable_buffers_1, 2> buffer_sequence_t;

  explicit frame_builder();
  virtual ~frame_builder();

  buffer_t get_header_buffer();

  bool is_body_read_required();

  buffer_sequence_t get_body_buffer();

  frame::ptr_t create_frame();

  void reset();

private:
  enum builder_state
  {
    invalid,
    valid_header,
    valid_both
  };

  void validate_header();
  void validate_body();

  builder_state m_state;
  boost::array<char, 8> m_header;

  bool m_valid_header;

  frame::frame_type m_type;
  uint16_t m_channel;
  boost::shared_ptr<scoped_buffer<char> > m_body;
  boost::array<uint8_t, 1> m_end;

};

} // namespace detail
} // namespace amqpp

#endif // _LIBAMQPP_FRAME_BUILDER_H_INCLUDED_