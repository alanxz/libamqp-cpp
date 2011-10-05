#ifndef AMQPP_IMPL_CHANNEL_IMPL_H
#define AMQPP_IMPL_CHANNEL_IMPL_H

#include "channel.h"
#include "frame.h"

#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>

namespace amqpp
{
namespace impl
{

class connection_impl;

class channel_impl : boost::noncopyable, public amqpp::channel, public boost::enable_shared_from_this<channel_impl>
{
public:

  enum channel_state
  {
    opening,
    open,
    close
  };

  typedef boost::shared_ptr<channel_impl> ptr_t;
  explicit channel_impl(uint16_t channel_id, const boost::shared_ptr<connection_impl>& connection, const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise);
  virtual ~channel_impl();

public: // Internal interface

  inline uint16_t get_channel_id() { return m_channel_id; }

  void process_frame(const detail::frame::ptr_t& frame);

  void process_open(const detail::frame::ptr_t& frame, const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise);
private:
  boost::shared_ptr<connection_impl> m_connection;
  const uint16_t m_channel_id;
  boost::function<void (const boost::shared_ptr<detail::frame>&)> m_continuation;
  channel_state m_state;
};

} // namespace impl
} // namespace amqpp

#endif // AMQPP_IMPL_CHANNEL_IMPL_H
