#ifndef _LIBAMQPCPP_WIREFORMAT_H_INCLUDED_
#define _LIBAMQPCPP_WIREFORMAT_H_INCLUDED_

#include "byteswap.h"

#include <boost/cstdint.hpp>

#include <istream>
#include <ostream>
#include <string>

namespace amqpp
{
namespace detail
{

class wireformat
{
	public:
		static inline void write_uint8(std::ostream& o, const uint8_t i)
		{
			o.write(reinterpret_cast<const char*>(&i), sizeof(i));
		}

		static inline void read_uint8(std::istream& o, uint8_t& i)
		{
			o.read(reinterpret_cast<char*>(&i), sizeof(i));
		}

		static inline void write_uint16(std::ostream& o, const uint16_t i)
		{
			uint16_t bs = byteswap(i);
			o.write(reinterpret_cast<const char*>(&bs), sizeof(bs));
		}

		static inline void read_uint16(std::istream& o, uint16_t& i)
		{
			o.read(reinterpret_cast<char*>(&i), sizeof(i));
			i = byteswap(i);
		}

		static inline void write_uint32(std::ostream& o, const uint32_t i)
		{
			uint32_t bs = byteswap(i);
			o.write(reinterpret_cast<const char*>(&bs), sizeof(bs));
		}

		static inline void read_uint32(std::istream& o, uint32_t& i)
		{
			o.read(reinterpret_cast<char*>(&i), sizeof(i));
			i = byteswap(i);
		}

		static inline void write_uint64(std::ostream& o, const uint64_t i)
		{
			uint64_t bs = byteswap(i);
			o.write(reinterpret_cast<const char*>(&bs), sizeof(bs));
		}

		static inline void read_uint64(std::istream& o, uint64_t& i)
		{
			o.read(reinterpret_cast<char*>(&i), sizeof(i));
			i = byteswap(i);
		}

		static void write_shortstring(std::ostream& o, const std::string& s);
		static void read_shortstring(std::istream& o, std::string& s);

		static void write_longstring(std::ostream& o, const std::string& s);
		static void read_longstring(std::istream& o, std::string& s);
};
} // namespace detail
} // namespace amqppp

#endif // _LIBAMQPCPP_WIREFORMAT_H_INCLUDED_
