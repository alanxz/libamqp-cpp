#include "properties.h"

#include "detail/frame.h"
#include "detail/frame_reader_adapter.h"

namespace amqpp
{
namespace detail
{

properties::ptr_t properties::read(const frame::ptr_t& f)
{
  return adapt_reader<properties>(f);
}

} // namespace detail
} // namespace amqpp