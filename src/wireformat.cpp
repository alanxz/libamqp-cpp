#include "wireformat.h"
#include "exception.h"
#include "byteswap.h"

#include <boost/cstdint.hpp>

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

void wireformat::write_table(std::ostream& o, const amqpp::table& t)
{
    //write_longstring(o, s);
}

amqpp::table wireformat::read_table(std::istream& i)
{
    std::istringstream is(read_longstring(i));
    amqpp::table t;
    while (!is.eof())
    {
      t.insert(wireformat::read_table_entry(is));
    }
    return t;
}

amqpp::table_entry wireformat::read_table_entry(std::istream& i)
{
  std::string field_name = read_shortstring(i);
  std::pair<table_entry::field_value_t, table_entry::field_type> field_value = read_field_value(i);
  return amqpp::table_entry(field_name, field_value.first, field_value.second);
}

std::pair<table_entry::field_value_t, table_entry::field_type> wireformat::read_field_value(std::istream& i)
{
  uint8_t field_type = read_uint8(i);

  switch (field_type)
  {
  case table_entry::boolean_type:
  {
    bool val (0 == read_uint8(i) ? false : true);
    return std::make_pair(table_entry::field_value_t(val), table_entry::boolean_type);
  }
  case table_entry::int8_type:
  {
    int8_t val = static_cast<int8_t>(read_uint8(i));
    return std::make_pair(table_entry::field_value_t(val), table_entry::int8_type);
  }
  case table_entry::uint8_type:
  {
    uint8_t val = read_uint8(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::uint8_type);
  }
  case table_entry::int16_type:
  {
    int16_t val = static_cast<int16_t>(read_uint16(i));
    return std::make_pair(table_entry::field_value_t(val), table_entry::int16_type);
  }
  case table_entry::uint16_type:
  {
    uint16_t val = read_uint16(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::uint16_type);
  }
  case table_entry::int32_type:
  {
    int32_t val = static_cast<int32_t>(read_uint32(i));
    return std::make_pair(table_entry::field_value_t(val), table_entry::int32_type);
  }
  case table_entry::uint32_type:
  {
    int32_t val = read_uint32(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::uint32_type);
  }
  case table_entry::int64_type:
  {
    int64_t val = static_cast<int64_t>(read_uint64(i));
    return std::make_pair(table_entry::field_value_t(val), table_entry::int64_type);
  }
  case table_entry::uint64_type:
  {
    uint64_t val = read_uint64(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::int64_type);
  }
  case table_entry::float_type:
  {
    float val = 0.f;
    i.read(reinterpret_cast<char*>(&val), sizeof(val));
    return std::make_pair(table_entry::field_value_t(val), table_entry::float_type);
  }
  case table_entry::double_type:
  {
    double val = 0.;
    i.read(reinterpret_cast<char*>(&val), sizeof(val));
    return std::make_pair(table_entry::field_value_t(val), table_entry::double_type);
  }
  case table_entry::decimal_type:
  {
    uint8_t mag = read_uint8(i);
    int32_t val = static_cast<int32_t>(read_uint32(i));
    return std::make_pair(table_entry::field_value_t(table_entry::decimal_t(mag, val)), table_entry::decimal_type);
  }
  case table_entry::shortstring_type:
  {
    std::string val = read_shortstring(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::shortstring_type);
  }
  case table_entry::longstring_type:
  {
    std::string val = read_longstring(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::longstring_type);
  }
  case table_entry::fieldarray_type:
  {
    std::string fieldarray = read_longstring(i);
    table_entry::field_array_t val;
    std::istringstream is(fieldarray);
    while (!is.eof())
    {
      val.push_back(read_field_value(is));
    }
    return std::make_pair(table_entry::field_value_t(val), table_entry::fieldarray_type);
  }
  case table_entry::timestamp_type:
  {
    uint64_t val = read_uint64(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::timestamp_type);
  }
  case table_entry::fieldtable_type:
  {
    table val = read_table(i);
    return std::make_pair(table_entry::field_value_t(val), table_entry::fieldtable_type);
  }
  case table_entry::void_type:
  {
    return std::make_pair(table_entry::field_value_t(table_entry::void_t()), table_entry::void_type);
  }
    break;
  default:
    throw std::runtime_error("Invalid field table type");
  }
}

} // namespace detail
} // namespace amqpp
