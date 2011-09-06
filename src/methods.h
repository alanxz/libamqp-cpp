#ifndef AMQPP_DETAIL_METHODS_H
#define AMQPP_DETAIL_METHODS_H

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <string>

namespace amqpp {
namespace detail {

template <uint16_t CLASS_ID, uint16_t METHOD_ID>
class method
{
public:
    inline uint16_t get_class_id() const { return CLASS_ID; }
    inline uint16_t get_method_id() const { return METHOD_ID; }

    virtual void write(std::ostream& o) const = 0;
};

class connection_start : public method<10,10>
{
public:
    static boost::shared_ptr<connection_start> read(std::istream& i);
    virtual void write(std::ostream& o) const;

    uint8_t version_major;
    uint8_t version_minor;
    std::string server_properties;
    std::string mechanisms;
    std::string locales;
};

} // namespace detail
} // namespace amqpp

#endif // AMQPP_DETAIL_METHODS_H
