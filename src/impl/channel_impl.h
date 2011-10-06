#ifndef AMQPP_IMPL_CHANNEL_IMPL_H
#define AMQPP_IMPL_CHANNEL_IMPL_H

#include "channel.h"
#include "connection_impl.h"
#include "exception.h"
#include "frame.h"
#include "frame_handler.h"

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

class channel_impl : public amqpp::channel, boost::noncopyable, public boost::enable_shared_from_this<channel_impl>, public detail::frame_handler
{
public:

  enum channel_state
  {
    opening,
    open,
    closed
  };

  typedef boost::shared_ptr<channel_impl> ptr_t;
  explicit channel_impl(uint16_t channel_id, const boost::shared_ptr<connection_impl>& connection, const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise);
  virtual ~channel_impl();

  virtual void close();

  virtual void declare_exchange();
  virtual void delete_exchange();

  virtual void declare_queue();
  virtual void delete_queue();

  virtual void bind_queue();
  virtual void unbind_queue();

  virtual void purge_queue();

public: // Internal interface

  typedef boost::promise<detail::method::ptr_t> rpc_promise_t;
  typedef boost::shared_ptr<rpc_promise_t> rpc_promise_ptr_t;
  typedef boost::unique_future<detail::method::ptr_t> rpc_future_t;

  inline uint16_t get_channel_id() { return m_channel_id; }

  virtual void process_frame(const detail::frame::ptr_t& frame);

  void process_open(const detail::frame::ptr_t& frame, const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise);
  

  template <class SentMethodT, class ResponseMethodT>
  boost::unique_future<typename ResponseMethodT::ptr_t> begin_rpc(const typename SentMethodT::ptr_t& method);

  template <class ResponseMethodT>
  void rpc_handler(const detail::frame::ptr_t& fr, boost::shared_ptr<boost::promise<typename ResponseMethodT::ptr_t> >& promise);

  void close_(const amqpp::channel_exception& e);
  void close_(uint16_t reply_code = 200, const std::string& reply_text = std::string(), uint16_t class_id = 0, uint16_t method_id = 0);
  void closed_handler(const detail::frame::ptr_t& fr);

private:
  boost::shared_ptr<connection_impl> m_connection;
  const uint16_t m_channel_id;
  boost::function<void (const boost::shared_ptr<detail::frame>&)> m_continuation;
  channel_state m_state;
};

template <class SentMethodT, class ResponseMethodT>
boost::unique_future<typename ResponseMethodT::ptr_t> channel_impl::begin_rpc(const typename SentMethodT::ptr_t& method)
{
  typedef boost::promise<typename ResponseMethodT::ptr_t> rpc_promise_t;
  typedef boost::shared_ptr<rpc_promise_t> rpc_promise_ptr_t;

  detail::frame::ptr_t out_frame = detail::frame::create_from_method(get_channel_id(), method);

  rpc_promise_ptr_t rpc_promise = boost::make_shared<rpc_promise_t>();

  m_continuation = boost::bind(&channel_impl::rpc_handler<ResponseMethodT>, this, _1, rpc_promise);

  m_connection->begin_write_method(get_channel_id(), method);

  return rpc_promise->get_future();
}

template <class ResponseMethodT>
void channel_impl::rpc_handler(const detail::frame::ptr_t& fr, boost::shared_ptr<boost::promise<typename ResponseMethodT::ptr_t> >& promise)
{
  try
  {
    if (fr->get_type() != detail::frame::METHOD_TYPE)
    {
      throw amqpp::connection_exception();
    }
    detail::method::ptr_t method = detail::method::read(fr);
    if (method->class_id() != ResponseMethodT::CLASS_ID ||
        method->method_id() != ResponseMethodT::METHOD_ID)
    {
      throw amqpp::connection_exception();
    }
    promise->set_value(detail::method_cast<ResponseMethodT>(method));
  }
  catch (std::exception& e)
  {
    promise->set_exception(boost::copy_exception(e));
    throw;
  }
}

} // namespace impl
} // namespace amqpp

#endif // AMQPP_IMPL_CHANNEL_IMPL_H
