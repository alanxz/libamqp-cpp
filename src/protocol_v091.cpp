#include "protocol_v091.h"

#include "wireformat.h"

#include <boost/array.hpp>

namespace amqpp
{



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

  char ret = io.peek();
  if (ret != 1)
  {
    // Remote end responded with something OTHER than the start of a frame
    // handle the error
  }
  
}


} // namespace amqpp