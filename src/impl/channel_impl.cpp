#include "channel_impl.h"

#include "connection_impl.h"

namespace amqpp {
namespace impl {

channel_impl::channel_impl(uint16_t& channel_id, boost::shared_ptr<connection_impl>& connection,
                           boost::shared_ptr<boost::promise<channel_impl::ptr_t> >& promise) :
  m_connection(connection), m_channel_id(channel_id), m_open_promise(promise)
{
}

} // namespace impl
} // namespace amqpp
