#include "methods.h"
#include "frame.h"

#include <boost/asio/buffer.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

namespace amqpp {
namespace detail {


boost::shared_ptr<method> method::read(const frame& f)
{
  typedef boost::iostreams::stream<boost::iostreams::array_source> array_istream;

  array_istream is(boost::asio::buffer_cast<char*>(f.get_payload_data()), f.get_payload_size());

  return read(is);
}

} // namespace detail
} // namespace amqpp
