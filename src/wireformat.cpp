#include "wireformat.h"

#include "byteswap.h"
#include "exception.h"

#include <boost/cstdint.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <cassert>
#include <sstream>


namespace amqpp
{
namespace detail
{

void wireformat::write_shortstring(std::ostream& o, const std::string& s)
{
	if (s.length() > 255)
	{
		throw std::logic_error("AMQPP: short strings can only be 255 bytes long");
	}

	uint8_t len = static_cast<uint8_t>(s.length());
	write_uint8(o, len);
	o.write(s.data(), s.length());
  
  if (!o.good())
  {
    throw std::runtime_error("Write failure");
  }
}

std::string wireformat::read_shortstring(std::istream& i)
{
	uint8_t len = read_uint8(i);
	if (!i.good())
	{
		// TODO: Throw a proper error
		throw std::runtime_error("Failure to read short string length");
	}
  
	char short_str[255];
	if (!(i.read(short_str, len).good()))
	{
		// TODO: Throw a proper error
		throw std::runtime_error("Failure to read short string data");
	}
	return std::string(short_str, len);
}

void wireformat::write_longstring(std::ostream& o, const std::string& s)
{
  if (s.length() > std::numeric_limits<uint32_t>::max())
  {
    throw std::logic_error("AMQPP: long strings can only be UINT32_MAX bytes long");
  }
	uint32_t len = static_cast<uint32_t>(s.length());
	write_uint32(o, len);
	o.write(s.data(), len);
  if (!o.good())
  {
    throw std::runtime_error("Write failure");
  }
}

std::string wireformat::read_longstring(std::istream& i)
{
	uint32_t len = read_uint32(i);
	if (!i.good())
	{
		// TODO: Throw a proper error
		throw std::runtime_error("Failure to read longstring length");
	}
  std::string s;
	s.reserve(len);
	s.clear();

	const uint32_t BUFFER_LENGTH = 1024;
	char buffer[BUFFER_LENGTH];

	uint32_t read_bytes = 0;
	while (read_bytes < len)
	{
		uint32_t to_read = (len - read_bytes) > BUFFER_LENGTH ? BUFFER_LENGTH : (len - read_bytes);
		read_bytes += to_read;
		if (!(i.read(buffer, to_read).good()))
		{
			// TODO: Throw a proper error
			throw std::runtime_error("Failure to read longstring data");
		}
		s.append(buffer, to_read);
	}
  return s;
}

void wireformat::write_table(std::ostream& o, const table& t)
{
  write_uint32(o, t.wireformat_size());
  const amqpp::table::table_impl_t map = t.get_map();

  for (amqpp::table::table_impl_t::const_iterator it = map.begin();
       it != map.end(); ++it)
  {
    write_table_entry(o, it->second);
  }
  if (!o.good())
  {
    throw std::runtime_error("Write Failure");
  }
}

void wireformat::write_table_entry(std::ostream& o, const table_entry& e)
{
  write_shortstring(o, e.get_key());
  write_table_value(o, e.get_data());
}

class table_value_writer : public boost::static_visitor<void>
{
private:
  std::ostream& os;
public:
  explicit table_value_writer(std::ostream& o) : os(o) {}

  void operator()(int8_t v) const
  {
    wireformat::write_uint8(os, table_entry::int8_type);
    wireformat::write_uint8(os, v);
  }

  void operator()(int16_t v) const
  {
    wireformat::write_uint8(os, table_entry::int16_type);
    wireformat::write_uint16(os, v);
  }

  void operator()(int32_t v) const
  {
    wireformat::write_uint8(os, table_entry::int32_type);
    wireformat::write_uint32(os, v);
  }

  void operator()(int64_t v) const
  {
    wireformat::write_uint8(os, table_entry::int64_type);
    wireformat::write_uint64(os, v);
  }

  void operator()(float v) const
  {
    wireformat::write_uint8(os, table_entry::float_type);
    wireformat::write_uint32(os, *reinterpret_cast<uint32_t*>(&v));
  }
  
  void operator()(double v) const
  {
    wireformat::write_uint8(os, table_entry::double_type);
    wireformat::write_uint64(os, *reinterpret_cast<uint64_t*>(&v));
  }

  void operator()(table_entry::decimal_t v) const
  {
    wireformat::write_uint8(os, table_entry::decimal_type);
    wireformat::write_uint8(os, v.first);
    wireformat::write_uint32(os, v.second);
  }

  void operator()(const std::string& v) const
  {
    wireformat::write_uint8(os, table_entry::longstring_type);
    wireformat::write_longstring(os, v);
  }

  void operator()(const table_entry::array_t& v) const
  {
    wireformat::write_uint8(os, table_entry::fieldarray_type);
    table_entry::array_t::const_iterator it;
    // Determine the wireformat size
    uint32_t size = 0;
    for (it = v.begin(); it != v.end(); ++it)
    {
      size += table_entry::wireformat_data_size(*it);
    }

    wireformat::write_uint32(os, size);
    for (it = v.begin(); it != v.end(); ++it)
    {
      wireformat::write_table_value(os, *it);
    }
  }

  void operator()(const table_entry::timestamp_t& v)
  {
    wireformat::write_uint8(os, table_entry::timestamp_type);
    wireformat::write_uint64(os, v);
  }

  void operator()(const table& v) const
  {
    wireformat::write_uint8(os, table_entry::fieldtable_type);
    wireformat::write_table(os, v);
  }

  void operator()(const table_entry::void_t) const
  {
    wireformat::write_uint8(os, table_entry::void_type);
  }

  void operator()(const table_entry::bytes_t& v) const
  {
    wireformat::write_uint8(os, table_entry::bytes_type);
    os.write(reinterpret_cast<const char*>(&v[0]), v.size());
    if (!os.good())
    {
      throw std::runtime_error("Write failed.");
    }
  }
};

void wireformat::write_table_value(std::ostream& o, const table_entry::field_value_t& d)
{
  boost::apply_visitor(table_value_writer(o), d);
}

amqpp::table wireformat::read_table(std::istream& i)
{
    std::istringstream is(read_longstring(i));
    amqpp::table t;
    while (is.peek() != std::char_traits<char>::eof())
    {
      t.insert(wireformat::read_table_entry(is));
    }
    return t;
}

amqpp::table_entry wireformat::read_table_entry(std::istream& i)
{
  std::string field_name = read_shortstring(i);
  table_entry::field_value_t field_value = read_field_value(i);
  return amqpp::table_entry(field_name, field_value);
}

table_entry::field_value_t wireformat::read_field_value(std::istream& i)
{
  uint8_t field_type = read_uint8(i);

  switch (field_type)
  {
  case table_entry::boolean_type:
  {
    bool val (0 == read_uint8(i) ? false : true);
    return table_entry::field_value_t(val);
  }
  case table_entry::int8_type:
  {
    int8_t val = static_cast<int8_t>(read_uint8(i));
    return table_entry::field_value_t(val);
  }
  case table_entry::int16_type:
  {
    int16_t val = static_cast<int16_t>(read_uint16(i));
    return table_entry::field_value_t(val);
  }
  case table_entry::int32_type:
  {
    int32_t val = static_cast<int32_t>(read_uint32(i));
    return table_entry::field_value_t(val);
  }
  case table_entry::int64_type:
  {
    int64_t val = static_cast<int64_t>(read_uint64(i));
    return table_entry::field_value_t(val);
  }
  case table_entry::float_type:
  {
    uint32_t preconvert = read_uint32(i);
    float val = *reinterpret_cast<float*>(&preconvert);
    return table_entry::field_value_t(val);
  }
  case table_entry::double_type:
  {
    uint64_t preconvert = read_uint64(i);
    double val = *reinterpret_cast<float*>(&preconvert);
    return table_entry::field_value_t(val);
  }
  case table_entry::decimal_type:
  {
    uint8_t mag = read_uint8(i);
    int32_t val = static_cast<int32_t>(read_uint32(i));
    return table_entry::field_value_t(table_entry::decimal_t(mag, val));
  }
  case table_entry::longstring_type:
  {
    std::string val = read_longstring(i);
    return table_entry::field_value_t(val);
  }
  case table_entry::fieldarray_type:
  {
    std::string fieldarray = read_longstring(i);
    table_entry::array_t val;
    std::istringstream is(fieldarray);
    while (!is.eof())
    {
      val.push_back(read_field_value(is));
    }
    return table_entry::field_value_t(val);
  }
  case table_entry::timestamp_type:
  {
    uint64_t val = read_uint64(i);
    return table_entry::field_value_t(val);
  }
  case table_entry::fieldtable_type:
  {
    table val = read_table(i);
    return table_entry::field_value_t(val);
  }
  case table_entry::void_type:
  {
    return table_entry::field_value_t(table_entry::void_t());
  }
  case table_entry::bytes_type:
  {
    uint32_t size = read_uint32(i);
    table_entry::bytes_t val(size);
    i.read(reinterpret_cast<char*>(&val[0]), size);
    if (!i.good())
    {
      throw std::runtime_error("Read failed.");
    }
    return table_entry::field_value_t(val);
  }
  default:
    throw std::runtime_error("Invalid field table type");
  }
}

} // namespace detail
} // namespace amqpp
