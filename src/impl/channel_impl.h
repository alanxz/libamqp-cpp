#ifndef AMQPP_IMPL_CHANNEL_IMPL_H
#define AMQPP_IMPL_CHANNEL_IMPL_H

#include "channel.h"

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>

namespace amqpp
{
namespace detail
{
class frame;
} // namespace detail
namespace impl
{

class connection_impl;

class channel_impl : boost::noncopyable, public amqpp::channel
{
public:
  typedef boost::shared_ptr<channel_impl> ptr_t;
  explicit channel_impl(uint16_t channel_id, boost::shared_ptr<connection_impl>& connection);
  virtual ~channel_impl();

  boost::unique_future<boost::shared_ptr<detail::frame> > rpc_reply;
  boost::promise<boost::shared_ptr<detail::frame> > rpc_promise;
private:
  boost::shared_ptr<connection_impl> m_connection;
  uint16_t m_channel_id;
};

} // namespace impl
} // namespace amqpp

#endif // AMQPP_IMPL_CHANNEL_IMPL_H
