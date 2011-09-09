#ifndef AMQPP_DETAIL_METHODS_H
#define AMQPP_DETAIL_METHODS_H

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <limits>

namespace amqpp {
namespace detail {

class method
{
public:
    static boost::shared_ptr<method> read(std::istream& i);

    virtual uint16_t get_class_id() const = 0;
    virtual uint16_t get_method_id() const = 0;

    virtual void write(std::ostream& o) const = 0;
    virtual std::string to_string() const = 0;
};

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

} // namespace detail
} // namespace amqpp

#endif // AMQPP_DETAIL_METHODS_H
