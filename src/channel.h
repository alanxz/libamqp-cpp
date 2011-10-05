#ifndef _LIBAMQPP_CHANNEL_H_INCLUDED_
#define _LIBAMQPP_CHANNEL_H_INCLUDED_

#include <boost/noncopyable.hpp>

namespace amqpp {

class channel : boost::noncopyable
{
  virtual void declare_exchange() = 0;
  virtual void delete_exchange() = 0;

  virtual void declare_queue() = 0;
  virtual void delete_queue() = 0;

  virtual void bind_queue() = 0;
  virtual void unbind_queue() = 0;

  virtual void purge_queue() = 0;

  virtual void close() = 0;
};

} // namespace amqpp

#endif // _LIBAMQPP_CHANNEL_H_INCLUDED_
