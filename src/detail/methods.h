#ifndef AMQPP_DETAIL_METHODS_H
#define AMQPP_DETAIL_METHODS_H

#include "export.h"

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <stdexcept>
#include <string>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4251 )
#endif

namespace amqpp {
namespace detail {

class frame;
typedef boost::shared_ptr<frame> frame_ptr_t;

class AMQPP_EXPORT method : boost::noncopyable
{
public:
  typedef boost::shared_ptr<method> ptr_t;

  static ptr_t read(const frame_ptr_t& f);

  // This is actually defined in methods.gen.cpp
  static ptr_t read(std::istream& i);

  virtual uint16_t class_id() const = 0;
  virtual uint16_t method_id() const = 0;

  virtual bool is_synchronous() const = 0;
  virtual bool has_content() const = 0;

  virtual void write(std::ostream& o) const = 0;
  virtual uint32_t get_serialized_size() const = 0;
  virtual std::string to_string() const = 0;
};

template <class T>
typename T::ptr_t method_cast(const method::ptr_t& m)
{
  typename T::ptr_t ret = boost::shared_dynamic_cast<T>(m);
  if (typename T::ptr_t() == ret)
  {
    throw std::runtime_error("Failure to cast method");
  }
  return ret;
}

} // namespace detail
} // namespace amqpp

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // AMQPP_DETAIL_METHODS_H
