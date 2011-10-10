#include "detail/channel0.h"

#include <stdexcept>

namespace amqpp
{
namespace detail
{

void channel0::process_frame(const frame::ptr_t& fr)
{
  throw std::logic_error("Channel0 process_frame not implemented.");
}

void channel0::close_async()
{
  throw std::logic_error("Channel0 close_async not implemented.");
}

} // namespace detail
} // namespace amqpp
