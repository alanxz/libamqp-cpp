#ifndef _LIBAMQPP_STRING_H_INCLUDED_
#define _LIBAMQPP_STRING_H_INCLUDED_

#include <boost/cstdint.hpp>

#include <limits>
#include <string>
#include <sstream>

namespace amqpp
{
namespace detail
{

/**
  * Validate short strings
  * short strings must be 255 characters or less in length
  * short strings must not contain any embedded nulls
  */
inline void validate_shortstring(const std::string& s)
{
  if (s.length() > std::numeric_limits<uint8_t>::max())
    throw std::runtime_error("Short string is longer than 255 bytes in length");
  if (s.find('\0') != std::string::npos)
    throw std::runtime_error("Short string may not contain any embedded '\\0' characters");
}

inline void validate_longstring(const std::string& s)
{
  if (s.length() > std::numeric_limits<uint32_t>::max())
    throw std::runtime_error("Long string cannot be longer than UINT_MAX bytes in length");
}

inline std::string print_string(const std::string& s)
{
  std::ostringstream o;
  o << s.length() << ":\"" << s << "\"";
  return o.str();
}

} // namespace detail
} // namespace amqpp
#endif // _LIBAMQPP_STRING_H_INCLUDED_