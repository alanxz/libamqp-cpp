#include "methods.h"
#include "detail/frame.h"

#include <boost/asio/buffer.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

namespace amqpp {
namespace detail {


method::ptr_t method::read(const frame::ptr_t f)
{
  typedef boost::iostreams::stream<boost::iostreams::array_source> array_istream;

  array_istream is(f->get_shared_buffer()->get_data(), f->get_shared_buffer()->get_size());

  return read(is);
}

} // namespace detail
} // namespace amqpp
