#ifndef AMQPP_DETAIL_METHODS_H
#define AMQPP_DETAIL_METHODS_H

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <string>

namespace amqpp {
namespace detail {

class method
{
public:
    static boost::shared_ptr<method> read(std::istream& i);

    virtual uint16_t get_class_id() const = 0;
    virtual uint16_t get_method_id() const = 0;

    virtual void write(std::ostream& o) const = 0;
    
};

class connection_start : public method
{
public:
  static const uint16_t CLASS_ID;
  static const uint16_t METHOD_ID;

  virtual uint16_t get_class_id() const { return CLASS_ID; }
  virtual uint16_t get_method_id() const { return METHOD_ID; }

    virtual void write(std::ostream& o) const;

    uint8_t version_major;
    uint8_t version_minor;
    std::string server_properties;
    std::string mechanisms;
    std::string locales;

protected:
    static boost::shared_ptr<connection_start> read(std::istream& i);
};

} // namespace detail
} // namespace amqpp

#endif // AMQPP_DETAIL_METHODS_H
