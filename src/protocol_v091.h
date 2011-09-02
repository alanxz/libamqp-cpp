#ifndef _LIBAMQPCPP_PROTOCOL_V091_H_INCLUDED_
#define _LIBAMQPCPP_PROTOCOL_V091_H_INCLUDED_

#include "export.h"

#include <boost/asio/buffer.hpp>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

#include <iosfwd>

namespace amqpp
{

class AMQPP_EXPORT protocol_v091 : boost::noncopyable
{
public:
  enum frame_t
  {
    any_frame = 0,
    method_frame = 1,
    header_frame = 2,
    body_frame = 3,
    heartbeat_frame = 4
  };

  struct frame 
  {
    uint32_t size;
    uint16_t channel;
    frame_t frame_type;
    
    boost::asio::mutable_buffer payload;
  };


  explicit protocol_v091();
  virtual ~protocol_v091();

  void handshake(std::iostream& io);

  frame read_frame(std::istream& is);
  void write_frame(std::ostream& os, const frame& frame);

  frame read_frame_header(std::istream& is);
  void validate_frame(std::istream& is, protocol_v091::frame& current_frame);

  frame_t get_frame_type(uint8_t i);

private:
  static const uint8_t FRAME_END;
};

} // namespace amqpp
#endif // _LIBAMQPCPP_PROTOCOL_V091_H_INCLUDED_