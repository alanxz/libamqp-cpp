#include "channel_impl.h"

#include "connection_impl.h"
#include "exception.h"
#include "frame.h"
#include "methods.h"

#include <boost/bind.hpp>

namespace amqpp {
namespace impl {

channel_impl::channel_impl(uint16_t channel_id, const boost::shared_ptr<connection_impl>& connection,
                           const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise) :
  m_connection(connection), m_channel_id(channel_id), m_state(opening)
{
  m_continuation = boost::bind(&channel_impl::process_open, this, _1, promise);
}

channel_impl::~channel_impl()
{
}

void channel_impl::close()
{
  throw std::logic_error("Not implemented");
}

void channel_impl::declare_exchange()
{
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

void channel_impl::process_frame(const detail::frame::ptr_t& frame)
{
  assert(frame->get_channel() == m_channel_id);

  try
  {
    m_continuation(frame);
  }
  catch (connection_exception)
  {
    m_state = close;
    throw;
  }
}

void channel_impl::process_open(const detail::frame::ptr_t& fr, const boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise)
{
  try
  {
    if (fr->get_type() == detail::frame::METHOD_TYPE)
    {
      throw amqpp::channel_exception();
    }

    detail::method::ptr_t method = detail::method::read(fr);
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

} // namespace impl
} // namespace amqpp
