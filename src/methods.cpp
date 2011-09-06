#include "methods.h"

#include "wireformat.h"

#include <boost/make_shared.hpp>
#include <istream>
#include <ostream>

namespace amqpp {
namespace detail {

boost::shared_ptr<connection_start> connection_start::read(std::istream& i)
{
    boost::shared_ptr<connection_start> resp = boost::make_shared<connection_start>();

    resp->version_major = wireformat::read_uint8(i);
    resp->version_minor = wireformat::read_uint8(i);
    resp->server_properties = wireformat::read_table(i);
    resp->mechanisms = wireformat::read_longstring(i);
    resp->locales = wireformat::read_longstring(i);

    return resp;
}

void connection_start::write(std::ostream& o) const
{
    wireformat::write_uint16(o, get_class_id());
    wireformat::write_uint16(o, get_method_id());
    wireformat::write_uint8(o, version_major);
    wireformat::write_uint8(o, version_minor);
    wireformat::write_table(o, server_properties);
    wireformat::write_longstring(o, mechanisms);
    wireformat::write_longstring(o, locales);
}

} // namespace detail
} // namespace amqpp
