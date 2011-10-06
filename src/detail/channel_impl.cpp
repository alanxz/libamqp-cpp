#include "channel_impl.h"

#include "detail/connection_impl.h"
#include "detail/frame.h"
#include "detail/methods.h"

#include <boost/bind.hpp>
#include <boost/thread/future.hpp>

namespace amqpp {
namespace detail {

channel_impl::channel_impl(uint16_t channel_id, const boost::shared_ptr<connection_impl>& connection,
                           const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise) :
  m_connection(connection), m_channel_id(channel_id), m_state(opening)
{
  m_continuation = boost::bind(&channel_impl::process_open, this, _1, promise);
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
  methods::exchange::declare_ok::ptr_t declare_ok = rpc_future.get();
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
  catch (amqpp::connection_exception)
  {
    // If we have a connection
    m_state = closed;
    throw;
  }
}

void channel_impl::process_open(const frame::ptr_t& fr, const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise)
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
      m_state = open;
      promise->set_value(shared_from_this());
    }
    else
    {
      throw amqpp::connection_exception();
    }
  }
  catch (std::exception& e)
  {
    promise->set_exception(boost::copy_exception(e));
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

  m_state = closed;
  m_continuation = boost::bind(&channel_impl::closed_handler, this, _1);

  m_connection->begin_write_method(get_channel_id(), close);
}

void channel_impl::closed_handler(const frame::ptr_t& fr)
{
  // Should this really do anything?
}

} // namespace detail
} // namespace amqpp
