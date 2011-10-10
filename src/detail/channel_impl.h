#ifndef AMQPP_IMPL_CHANNEL_IMPL_H
#define AMQPP_IMPL_CHANNEL_IMPL_H

#include "channel.h"
#include "exception.h"
#include "detail/frame.h"
#include "detail/frame_handler.h"

#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>

namespace amqpp
{
namespace detail
{

class connection_impl;

class channel_impl : public amqpp::channel, boost::noncopyable, public boost::enable_shared_from_this<channel_impl>, public frame_handler
{
public:

  enum channel_state
  {
    opening,
    open,
    closed
  };

  typedef boost::shared_ptr<channel_impl> ptr_t;
  explicit channel_impl(uint16_t channel_id, const boost::shared_ptr<connection_impl>& connection);
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

  typedef boost::promise<method::ptr_t> rpc_promise_t;
  typedef boost::shared_ptr<rpc_promise_t> rpc_promise_ptr_t;
  typedef boost::unique_future<method::ptr_t> rpc_future_t;

  typedef boost::promise<bool>        channel_opened_promise_t;
  typedef boost::unique_future<bool>  channel_opened_future_t;

  inline uint16_t get_channel_id() { return m_channel_id; }

  inline channel_opened_future_t get_channel_opened_future() { return m_channel_opened_promise.get_future(); }

  virtual void process_frame(const frame::ptr_t& frame);

  void process_open(const frame::ptr_t& frame);

  virtual void close_async();

  template <class SentMethodT, class ResponseMethodT>
  boost::unique_future<typename ResponseMethodT::ptr_t> begin_rpc(const typename SentMethodT::ptr_t& method);

  template <class ResponseMethodT>
  void rpc_handler(const frame::ptr_t& fr, boost::shared_ptr<boost::promise<typename ResponseMethodT::ptr_t> >& promise);

  void close_(const amqpp::channel_exception& e);
  void close_(uint16_t reply_code = 200, const std::string& reply_text = std::string(), uint16_t class_id = 0, uint16_t method_id = 0);
  void closed_handler(const frame::ptr_t& fr);


private:
  boost::shared_ptr<connection_impl> m_connection;
  const uint16_t m_channel_id;
  boost::function<void (const boost::shared_ptr<frame>&)> m_continuation;

  channel_opened_promise_t m_channel_opened_promise;

  boost::promise<int> m_channel_closed_promise;
  boost::unique_future<int> m_channel_closed_future;
};


} // namespace detail
} // namespace amqpp

#endif // AMQPP_IMPL_CHANNEL_IMPL_H
