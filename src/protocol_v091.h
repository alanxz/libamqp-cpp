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

  explicit protocol_v091();
  virtual ~protocol_v091();

  void handshake(std::iostream& io);

private:
};

} // namespace amqpp
#endif // _LIBAMQPCPP_PROTOCOL_V091_H_INCLUDED_