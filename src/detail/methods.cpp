#include "methods.h"
#include "detail/frame.h"

#include "detail/frame_reader_adapter.h"

namespace amqpp {
namespace detail {

method::ptr_t method::read(const frame_ptr_t& f)
{
  return adapt_reader<method>(f);
}

} // namespace detail
} // namespace amqpp
