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

} // namespace detail
} // namespace amqpp
