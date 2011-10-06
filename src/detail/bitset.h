#ifndef _LIBAMQPP_BITSET_H_INCLUDED_
#define _LIBAMQPP_BITSET_H_INCLUDED_

#include <boost/cstdint.hpp>

#include <cassert>

namespace amqpp
{
namespace detail
{

template <class T>
inline T set_bit(T bitset, bool val, uint8_t position)
{
  assert(position < sizeof(T) * 8);
  if (val)
    return bitset | (1 << position);
  else
    return bitset & ~(1 << position);
}

template <class T>
inline bool get_bit(T bitset, uint8_t position)
{
  assert(position < sizeof(T) * 8);
  return (bitset & (1 << position)) != 0;
}

} // namespace detail
} // namespace amqpp

#endif // _LIBAMQPP_BITSET_H_INCLUDED_