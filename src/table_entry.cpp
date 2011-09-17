#include "table_entry.h"
#include "string_utils.h"

#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/string.hpp>

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <iomanip>
#include <istream>
#include <ostream>

namespace amqpp {

namespace detail
{
  namespace mpl = boost::mpl;
  typedef mpl::map<
    mpl::pair<bool,                    mpl::int_<table_entry::boolean_type> >,
    mpl::pair<int8_t,                  mpl::int_<table_entry::int8_type> >,
    mpl::pair<int16_t,                 mpl::int_<table_entry::int16_type> >,
    mpl::pair<int32_t,                 mpl::int_<table_entry::int32_type> >,
    mpl::pair<int64_t,                 mpl::int_<table_entry::int64_type> >,
    mpl::pair<float,                   mpl::int_<table_entry::float_type> >,
    mpl::pair<double,                  mpl::int_<table_entry::double_type> >,
    mpl::pair<table_entry::decimal_t,  mpl::int_<table_entry::decimal_type> >,
    mpl::pair<std::string,             mpl::int_<table_entry::longstring_type> >,
    mpl::pair<table_entry::array_t,    mpl::int_<table_entry::fieldarray_type> >,
    mpl::pair<table_entry::timestamp_t,mpl::int_<table_entry::timestamp_type> >,
    mpl::pair<table,                   mpl::int_<table_entry::fieldtable_type> >,
    mpl::pair<table_entry::void_t,     mpl::int_<table_entry::void_type> >,
    mpl::pair<table_entry::bytes_t,    mpl::int_<table_entry::bytes_type> >
  > field_type_to_field_value_t_map;


  class type_getter : public boost::static_visitor<table_entry::field_type>
  {
    public:

    template <class T>
    table_entry::field_type operator()(const T&) const
    {
      typedef typename mpl::at<field_type_to_field_value_t_map, T>::type enum_type;
      return table_entry::field_type(enum_type::value);
    }
  };

  class datatype_size_counter : public boost::static_visitor<uint32_t>
  {
  public:
    template <class T>
    uint32_t operator()(const T&) const
    {
      return sizeof(T);
    }

    uint32_t operator()(const bool) const
    {
      return sizeof(uint8_t);
    }

    uint32_t operator()(const std::string& v) const
    {
      return detail::wireformat_size_longstring(v);
    }

    uint32_t operator()(const table_entry::array_t& a) const
    {
      uint32_t size = 0;
      for (table_entry::array_t::const_iterator it = a.begin();
           it != a.end(); ++it)
      {
        size += table_entry::wireformat_data_size(*it);
      }
      return size;
    }

    uint32_t operator()(const table& t) const
    {
      return t.wireformat_size();
    }

    uint32_t operator()(const table_entry::void_t) const
    {
      return 0;
    }

    uint32_t operator()(const table_entry::bytes_t& v) const
    {
      return sizeof(uint32_t) + sizeof(uint8_t) * static_cast<uint32_t>(v.size());
    }
  };

  typedef mpl::map<
    mpl::pair<int8_t,                  mpl::string<'<int','8>: '> >,
    mpl::pair<int16_t,                 mpl::string<'<int','16>:',' '> >,
    mpl::pair<int32_t,                 mpl::string<'<int','32>:',' '> >,
    mpl::pair<int64_t,                 mpl::string<'<int','64>:',' '> >,
    mpl::pair<float,                   mpl::string<'<flo','at>:',' '> >,
    mpl::pair<double,                  mpl::string<'<dou','ble>',': '> >,
    mpl::pair<table_entry::timestamp_t,mpl::string<'<tim','esta','mp>:',' '> >
  > field_type_to_string_t;
  
  class data_stringifier : public boost::static_visitor<void>
  {
    private:
      std::ostream& os;
    public:
      explicit data_stringifier(std::ostream& o) : os(o) {}

      template <class T>
      void operator()(T d) const
      {
        typedef typename mpl::at<field_type_to_string_t, T>::type field_type_name;
        os << mpl::c_str<field_type_name>::value << d;
      }

      void operator()(bool d) const
      {
        os << "<boolean>: " << (d ? "true" : "false");
      }

      void operator()(table_entry::decimal_t d) const
      {
        os << "<decimal>: mag: " << static_cast<unsigned int>(d.first) << " val: " << d.second;
      }

      void operator()(const std::string& s) const
      {
        os << "<longstring>: len:" << s.length() << " \"" << s << "\"";
      }

      void operator()(const table_entry::array_t& a) const
      {
        os << "<fieldarray>: len:" << a.size() << " {";
        for (table_entry::array_t::const_iterator it = a.begin();
             it != a.end(); ++it)
        {
          table_entry::value_to_string(os, *it);
          os << ", ";
        }
        os << "}";
      }

      void operator()(const table& t) const
      {
        os << "<fieldtable>: " << t.to_string();
      }

      void operator()(table_entry::void_t) const
      {
        os << "<void>";
      }

      void operator()(const table_entry::bytes_t& d) const
      {
        os << "<bytes>: { len: " << d.size();
        os << std::hex;
        for (table_entry::bytes_t::const_iterator it = d.begin();
             it != d.end(); ++it)
        {
          os << static_cast<int>(*it);
        }
        os << std::dec;
      }
  };
}

table_entry::table_entry(const std::string& key, bool value) :
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}

table_entry::table_entry(const std::string& key, int8_t value) :
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std::runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, int16_t value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, int32_t value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, int64_t value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, float value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, double value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, decimal_t value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, const std::string& value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, const array_t& value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, timestamp_t value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, const table& value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, void_t value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}
table_entry::table_entry(const std::string& key, bytes_t value):
  m_key(key), m_data(field_value_t(value))
{
  if (!validate_key_name(m_key))
  {
    throw std:: runtime_error("Table entry key name is not valid.");
  }
}

table_entry::table_entry(const std::string& key, const field_value_t& data) :
  m_key(key), m_data(data)
{
  if (!validate_key_name(m_key))
  {
    throw std::runtime_error("Key name is invalid");
  }
}

table_entry::~table_entry()
{
}

table_entry::field_type table_entry::get_type() const
{
  return boost::apply_visitor(detail::type_getter(), m_data);
}

bool table_entry::validate_key_name(const std::string& key)
{
  if (key.length() > 128)
  {
    return false;
  }
  return true;
}

std::string table_entry::to_string() const
{
  std::ostringstream os;
  os << get_key();

  value_to_string(os, get_data());

  return os.str();
}

void table_entry::value_to_string(std::ostream& os, const field_value_t& data)
{
  boost::apply_visitor(detail::data_stringifier(os), data);
}

uint32_t table_entry::wireformat_size() const
{
  uint32_t size = detail::wireformat_size_shortstring(m_key);
  size += wireformat_data_size(m_data);
  return size;
}

uint32_t table_entry::wireformat_data_size(const table_entry::field_value_t& data)
{
  uint32_t size = sizeof(uint8_t); // size of datatype
  size += boost::apply_visitor(detail::datatype_size_counter(), data);
  return size;
}

} // namespace amqpp
