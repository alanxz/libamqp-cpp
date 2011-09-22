#ifndef _LIBAMQPP_FRAME_WRITER_H_INCLUDED_
#define _LIBAMQPP_FRAME_WRITER_H_INCLUDED_

#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace amqpp
{
namespace detail
{

class frame;

class frame_writer : boost::noncopyable
{
public:
  typedef boost::array<boost::asio::const_buffer, 3> buffer_sequence_t;
  

  buffer_sequence_t get_sequence(const boost::shared_ptr<frame>& frame);

private:
  boost::array<uint8_t, 7> m_header;
  static const boost::array<uint8_t, 1> FOOTER;
};

} // namespace detail
} // namespace amqpp
#endif // _LIBAMQPP_FRAME_WRITER_H_INCLUDED_
