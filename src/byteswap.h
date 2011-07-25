#ifndef _AMQPPP_BYTESWAP_H_INCLUDED_
#define _AMQPPP_BYTESWAP_H_INCLUDED_

#include <boost/detail/endian.hpp>
#include <boost/cstdint.hpp>

#ifdef _MSC_VER
# include <intrin.h>
#endif


namespace amqpp
{
namespace detail
{

#if defined(BOOST_BIG_ENDIAN)
# define byteswap(v) v
#elif defined(BOOST_LITTLE_ENDIAN)
inline uint16_t byteswap(uint16_t v)
{
#if defined(_MSC_VER)
	return _byteswap_ushort(v);
#elif defined(__GNUC__)
	uint16_t ret;
	swab(reinterpret_cast<char*>(&v), reinterpret_cast<char*>(&ret), 2);
	return ret;
#else
	return ((0x00FF & v) << 8) |
		   ((0xFF00 & v) >> 8);
#endif
}

inline uint32_t byteswap(uint32_t v)
{
#if defined(_MSC_VER)
	return _byteswap_ulong(v);
#elif defined(__GNUC__)
	return __builtin_bswap32(v);
#else
	return ((0x000000FF & v) << 24) |
		   ((0x0000FF00 & v) << 8 ) |
		   ((0x00FF0000 & v) >> 8 ) |
		   ((0xFF000000 & v) >> 24);
#endif
}

inline uint64_t byteswap(uint64_t v)
{
#if defined(_MSC_VER)
	return _byteswap_uint64(v);
#elif defined(__GNUC__)
	return __builtin_bswap64(v);
#else
	return ((0x00000000000000FF & v) << 56) |
		   ((0x000000000000FF00 & v) << 40) |
		   ((0x0000000000FF0000 & v) << 24) |
		   ((0x00000000FF000000 & v) << 8 ) |
		   ((0x000000FF00000000 & v) >> 8 ) |
		   ((0x0000FF0000000000 & v) >> 24) |
		   ((0x00FF000000000000 & v) >> 40) |
		   ((0xFF00000000000000 & v) >> 56);
#endif
}
#else
# error "Don't know what endianness this system is"
#endif 

} // namespace impl
} // namespace amqppp

#endif // _AMQPPP_BYTESWAP_H_INCLUDED_
