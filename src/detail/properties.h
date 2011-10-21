#ifndef AMQPP_PROPERTIES_H_INCLUDED_H
#define AMQPP_PROPERTIES_H_INCLUDED_H

#include "export.h"

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>

namespace amqpp
{
namespace detail
{

class frame;
typedef boost::shared_ptr<frame> frame_ptr_t;

class AMQPP_EXPORT properties : boost::noncopyable
{
public:
  typedef boost::shared_ptr<properties> ptr_t;
  virtual ~properties() {}

  virtual uint16_t class_id() const = 0;

  static ptr_t read(const frame_ptr_t& f);

  // This is actually defined in methods.gen.cpp
  static ptr_t read(std::istream& i);

  virtual void write(std::ostream& o) const = 0;
  virtual uint32_t get_serialized_size() const = 0;
  virtual std::string to_string() const = 0;

};

template <class T>
typename T::ptr_t properties_cast(const properties::ptr_t& m)
{
  typename T::ptr_t ret = boost::shared_dynamic_cast<T>(m);
  if (typename T::ptr_t() == ret)
  {
    throw std::runtime_error("Failure to cast properties");
  }
  return ret;
}

}
} // namespace amqpp
#endif // AMQPP_PROPERTIES_H_INCLUDED_H