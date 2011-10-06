#ifndef _LIBAMQPP_FRAME_H_INCLUDED_
#define _LIBAMQPP_FRAME_H_INCLUDED_

#include "export.h"
#include "detail/scoped_buffer.h"
#include "detail/methods.h"
#include "methods.gen.h"

#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif

namespace amqpp
{
namespace detail
{

class AMQPP_EXPORT frame : boost::noncopyable, public boost::enable_shared_from_this<frame>
{
public:
  typedef boost::array<boost::asio::const_buffer, 3> buffer_sequence_t;

  static const boost::array<char, 1> END_BUFFER;

  static const uint8_t FRAME_END;
  enum frame_type 
  {
    METHOD_TYPE = detail::FRAME_METHOD,
    HEADER_TYPE = detail::FRAME_HEADER,
    BODY_TYPE = detail::FRAME_BODY,
    HEARTBEAT_TYPE = detail::FRAME_HEARTBEAT
  };

  typedef boost::shared_ptr<frame> ptr_t;
  typedef boost::shared_ptr<amqpp::detail::scoped_buffer<char> > shared_buffer_t;

  static frame_type get_frame_type(const uint8_t val);

  static ptr_t create_from_method(uint16_t channel, const detail::method::ptr_t method);

  frame(frame_type type, uint16_t channel, const shared_buffer_t& buffer);
  frame(frame_type type, uint16_t channel, uint32_t payload_size);
  virtual ~frame();

  inline frame_type get_type() const { return m_type; }
  inline uint16_t get_channel() const { return m_channel; }
  inline shared_buffer_t get_shared_buffer() const { return m_shared_buffer; }

private:
  shared_buffer_t m_shared_buffer;
  uint16_t m_channel;
  frame_type m_type;
};

} // namespace detail
} // namespace amqpp

#ifdef _MSC_VER
# pragma warning ( pop )
#endif
#endif // _LIBAMQPP_FRAME_H_INCLUDED_
