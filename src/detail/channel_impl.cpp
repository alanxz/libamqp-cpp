#include "channel_impl.h"

#include "detail/connection_impl.h"
#include "detail/frame.h"
#include "detail/methods.h"

#include <boost/bind.hpp>
#include <boost/thread/future.hpp>

namespace amqpp {
namespace detail {

template <class SentMethodT, class ResponseMethodT>
boost::unique_future<typename ResponseMethodT::ptr_t> channel_impl::begin_rpc(const typename SentMethodT::ptr_t& method)
{
  typedef boost::promise<typename ResponseMethodT::ptr_t> rpc_promise_t;
  typedef boost::shared_ptr<rpc_promise_t> rpc_promise_ptr_t;

  if (!m_channel_closed_future.is_ready())
  {
    frame::ptr_t out_frame = frame::create_from_method(get_channel_id(), method);

    rpc_promise_ptr_t rpc_promise = boost::make_shared<rpc_promise_t>();

    m_continuation = boost::bind(&channel_impl::rpc_handler<ResponseMethodT>, this, _1, rpc_promise);

    m_connection->begin_write_method(get_channel_id(), method);

    return rpc_promise->get_future();
  }
  else
  {
    throw std::runtime_error("Channel closed");
  }
}

template <class ResponseMethodT>
void channel_impl::rpc_handler(const frame::ptr_t& fr, boost::shared_ptr<boost::promise<typename ResponseMethodT::ptr_t> >& promise)
{
  try
  {
    if (fr->get_type() != frame::METHOD_TYPE)
    {
      throw amqpp::connection_exception();
    }
    method::ptr_t method = method::read(fr);
    if (method->class_id() != ResponseMethodT::CLASS_ID ||
        method->method_id() != ResponseMethodT::METHOD_ID)
    {
      throw amqpp::connection_exception();
    }
    promise->set_value(method_cast<ResponseMethodT>(method));
  }
  catch (std::exception& e)
  {
    promise->set_exception(boost::copy_exception(e));
    throw;
  }
}

channel_impl::channel_impl(uint16_t channel_id, const boost::shared_ptr<connection_impl>& connection) :
  m_connection(connection), m_channel_id(channel_id)
{
  // Need to hold on to a reference to this() until the callback happens, otherwise it gets destructed too soon
  m_continuation = boost::bind(&channel_impl::process_open, this, _1);
}

channel_impl::~channel_impl()
{
  close_();
}

void channel_impl::close()
{
  throw std::logic_error("Not implemented");
}

void channel_impl::declare_exchange()
{
  methods::exchange::declare::ptr_t declare = methods::exchange::declare::create();
  boost::unique_future<methods::exchange::declare_ok::ptr_t> rpc_future = 
    begin_rpc<methods::exchange::declare, methods::exchange::declare_ok>(declare);

  size_t future = boost::wait_for_any(rpc_future, m_channel_closed_future);
  if (0 == future)
  {
    methods::exchange::declare_ok::ptr_t declare_ok = rpc_future.get();
  }
  else
  {
    throw std::runtime_error("Channel is closed");
  }
}


void channel_impl::delete_exchange()
{
}

void channel_impl::declare_queue()
{
}

void channel_impl::delete_queue()
{
}

void channel_impl::bind_queue()
{
}

void channel_impl::unbind_queue()
{
}

void channel_impl::purge_queue()
{
}

void channel_impl::process_frame(const frame::ptr_t& frame)
{
  assert(frame->get_channel() == m_channel_id);

  try
  {
    m_continuation(frame);
  }
  catch (amqpp::channel_exception& e)
  {
    close_(e);
  }
}

void channel_impl::process_open(const frame::ptr_t& fr)
{
  try
  {
    if (fr->get_type() != frame::METHOD_TYPE)
    {
      throw amqpp::connection_exception();
    }

    method::ptr_t method = method::read(fr);
    if (method->class_id() == methods::channel::CLASS_ID &&
        method->method_id() == methods::channel::open_ok::METHOD_ID)
    {
      m_channel_opened_promise.set_value(true);
    }
    else
    {
      throw amqpp::connection_exception();
    }
  }
  catch (std::exception& e)
  {
    m_channel_opened_promise.set_exception(boost::copy_exception(e));
    throw;
  }
}

void channel_impl::close_(const amqpp::channel_exception& e)
{
  close_(e.reply_code(), e.reply_text(), e.class_id(), e.method_id());
}

void channel_impl::close_(uint16_t reply_code, const std::string& reply_text, uint16_t class_id, uint16_t method_id)
{
  methods::channel::close::ptr_t close = methods::channel::close::create();
  close->set_reply_code(reply_code);
  close->set_reply_text(reply_text);
  close->set_class_id(class_id);
  close->set_method_id(method_id);

  m_continuation = boost::bind(&channel_impl::closed_handler, this, _1);

  m_connection->begin_write_method(get_channel_id(), close);
}

void channel_impl::closed_handler(const frame::ptr_t& fr)
{
  // Should this really do anything?
}

void channel_impl::close_async()
{
  m_channel_closed_promise.set_value(1);
}

} // namespace detail
} // namespace amqpp
