#ifndef AMQPP_DETAIL_METHODS_H
#define AMQPP_DETAIL_METHODS_H

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <iosfwd>

namespace amqpp {
namespace detail {

class frame;

class method
{
public:
    static boost::shared_ptr<method> read(const frame& f);
    // This is actually defined in methods.gen.cpp
    static boost::shared_ptr<method> read(std::istream& i);

    virtual uint16_t class_id() const = 0;
    virtual uint16_t method_id() const = 0;

    virtual void write(std::ostream& o) const = 0;
    virtual uint32_t get_serialized_size() const = 0;
    virtual std::string to_string() const = 0;
};

} // namespace detail
} // namespace amqpp

#endif // AMQPP_DETAIL_METHODS_H
