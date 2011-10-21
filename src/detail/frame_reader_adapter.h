#ifndef AMQPP_FRAME_READER_ADAPTER_H_INCLUDED_
#define AMQPP_FRAME_READER_ADAPTER_H_INCLUDED_

#include "frame.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>

namespace amqpp
{
namespace detail
{

template <class StreamReader>
typename StreamReader::ptr_t adapt_reader(const frame::ptr_t f)
{
  typedef boost::iostreams::stream<boost::iostreams::array_source> array_istream;

  array_istream is(f->get_shared_buffer()->get_data(), f->get_shared_buffer()->get_size());

  return StreamReader::read(is);
}

} // namespace detail
} // namespace amqpp
#endif // AMQPP_FRAME_READER_ADAPTER_H_INCLUDED_