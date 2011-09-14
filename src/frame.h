#ifndef _LIBAMQPP_FRAME_H_INCLUDED_
#define _LIBAMQPP_FRAME_H_INCLUDED_

#include "scoped_buffer.h"
#include "methods.gen.h"
#include <boost/asio/buffer.hpp>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>

namespace amqpp
{
namespace detail
{

class frame : boost::noncopyable
{
public:
  static const uint8_t FRAME_END;
  enum frame_type 
  {
    METHOD_TYPE = detail::FRAME_METHOD,
    HEADER_TYPE = detail::FRAME_HEADER,
    BODY_TYPE = detail::FRAME_BODY,
    HEARTBEAT_TYPE = detail::FRAME_HEARTBEAT
  };

  typedef boost::shared_ptr<amqpp::detail::scoped_buffer<char> > shared_buffer_t;

  static frame_type get_frame_type(const uint8_t val);

  static boost::shared_ptr<frame> read_frame(std::istream& i);
  static void write_frame(const frame& f, std::ostream& o);

  frame(frame_type type, uint16_t channel, const boost::asio::mutable_buffer& payload);
  frame(frame_type type, uint16_t channel, uint32_t payload_size);
  frame(frame_type type, uint16_t channel, boost::shared_ptr<scoped_buffer<char> >& shared_payload);
  virtual ~frame();

  inline frame_type get_type() const { return m_type; }
  inline void set_type(frame_type val) { m_type = val; }

  inline uint16_t get_channel() const { return m_channel; }
  inline void set_channel(uint16_t val) { m_channel = val; }

  inline uint32_t get_payload_size() const { return boost::asio::buffer_size(m_buffer); }
  inline boost::asio::mutable_buffer get_payload_data() const { return m_buffer; }

  inline bool have_shared_buffer() const { return shared_buffer_t() == m_shared_buffer; }
  inline shared_buffer_t get_shared_buffer() const { return m_shared_buffer; }

private:
  frame_type m_type;
  uint16_t m_channel;
  boost::asio::mutable_buffer m_buffer;

  shared_buffer_t m_shared_buffer;
};

} // namespace detail
} // namespace amqpp
#endif // _LIBAMQPP_FRAME_H_INCLUDED_