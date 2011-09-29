#ifndef AMQPP_IMPL_CHANNEL_IMPL_H
#define AMQPP_IMPL_CHANNEL_IMPL_H

#include "channel.h"
#include "frame.h"

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
  explicit channel_impl(uint16_t& channel_id, boost::shared_ptr<connection_impl>& connection, boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise);
  virtual ~channel_impl();


public: // Internal interface

  inline uint16_t get_channel_id() { return m_channel_id; }

  void process_frame(const detail::frame::ptr_t& frame);

  boost::unique_future<boost::shared_ptr<detail::frame> > rpc_reply;
  boost::promise<boost::shared_ptr<detail::frame> > rpc_promise;
private:
  boost::shared_ptr<connection_impl> m_connection;
  const uint16_t m_channel_id;
  boost::shared_ptr<boost::promise<channel_impl::ptr_t> > m_open_promise;
};

} // namespace impl
} // namespace amqpp

#endif // AMQPP_IMPL_CHANNEL_IMPL_H
