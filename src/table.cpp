#include "table.h"
#include "string_utils.h"

#include <boost/mpl/at.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/static_visitor.hpp>

#include <sstream>
#include <stdexcept>

namespace amqpp
{

void table::insert(const table_entry& data)
{
  m_table.insert(std::make_pair(data.get_key(), data));
}

std::string table::to_string() const
{
  std::ostringstream os;
  os << "{ len: " << m_table.size();
  for (table_impl_t::const_iterator it = m_table.begin();
        it != m_table.end(); ++it)
  {
    os << ", " << it->second.to_string();
  }
  os << "}";
  return os.str();
}
table_entry::table_entry(const std::string& key, const field_value_t& data, field_type data_type) :
  m_key(key), m_data(data), m_type(data_type)
{
  if (!validate_key_name(m_key))
  {
    throw std::runtime_error("Key name is invalid");
  }
  if (!validate_data_type(m_data, m_type))
  {
    throw std::runtime_error("Invalid field type");
  }
}

table_entry::~table_entry()
{
}

bool table_entry::validate_key_name(const std::string& key)
{
  if (key.length() > 128)
  {
    return false;
  }
  return true;
}

namespace detail
{
  namespace mpl = boost::mpl;
  typedef mpl::map<
    mpl::pair<bool,                    mpl::int_<table_entry::boolean_type> >,
    mpl::pair<int8_t,                  mpl::int_<table_entry::int8_type> >,
    mpl::pair<uint8_t,                 mpl::int_<table_entry::uint8_type> >,
    mpl::pair<int16_t,                 mpl::int_<table_entry::int16_type> >,
    mpl::pair<uint16_t,                mpl::int_<table_entry::uint16_type> >,
    mpl::pair<int32_t,                 mpl::int_<table_entry::int32_type> >,
    mpl::pair<uint32_t,                mpl::int_<table_entry::uint32_type> >,
    //mpl::pair<int64_t,                 mpl::int_<table_entry::int64_type> >,
    mpl::pair<uint64_t,                mpl::int_<table_entry::uint64_type> >,
    mpl::pair<float,                   mpl::int_<table_entry::float_type> >,
    mpl::pair<double,                  mpl::int_<table_entry::double_type> >,
    mpl::pair<table_entry::decimal_t,  mpl::int_<table_entry::decimal_type> >,
    //mpl::pair<shortstring_t,         mpl::int_<table_entry::shortstring_type> >,
    //mpl::pair<longstring_t,          mpl::int_<table_entry::longstring_type> >,
    mpl::pair<table_entry::field_array_t, mpl::int_<table_entry::fieldarray_type> >,
    //boost::mpl::pair<timestamp_t,    mpl::int_<table_entry::timestamp_type> >,
    mpl::pair<table,                   mpl::int_<table_entry::fieldtable_type> >,
    mpl::pair<table_entry::void_t,     mpl::int_<table_entry::void_type> >
  > field_type_to_field_value_t_map;

  class datatype_validator : public boost::static_visitor<bool>
  {
  private:
    const table_entry::field_type m_type;
  public:
    explicit datatype_validator(table_entry::field_type type) : m_type(type) {}

    template <class T>
    bool operator()(const T&) const
    {
      typedef typename mpl::at<field_type_to_field_value_t_map, T>::type enum_type;

      if (enum_type::value == m_type) 
        return true;
      else
        return false;
    }

    bool operator()(const std::string&) const
    {
      if (table_entry::shortstring_type == m_type ||
          table_entry::longstring_type == m_type)
        return true;
      else
        return false;
    }

    bool operator()(const int64_t) const
    {
      if (table_entry::int64_type == m_type ||
          table_entry::timestamp_type == m_type)
        return true;
      else
        return false;
    }

  };
} // namespace detail;

bool table_entry::validate_data_type(const field_value_t& data, field_type type)
{
  detail::datatype_validator validator(type);
  return boost::apply_visitor(detail::datatype_validator(type), data);
}

std::string table_entry::to_string() const
{
  std::ostringstream os;
  os << get_key();

  value_to_string(os, get_type(), get_data());

  return os.str();
}

void table_entry::value_to_string(std::ostream& os, field_type type, const field_value_t& data)
{
  switch (type)
  {
  case boolean_type:
    os << "<boolean>: " << (boost::get<bool>(data) ? "true" : "false");
    break;
  case int8_type:
    os << "<int8>: " << static_cast<int>(boost::get<int8_t>(data));
    break;
  case uint8_type:
    os << "<uint8>: " << static_cast<unsigned int>(boost::get<uint8_t>(data));
    break;
  case int16_type:
    os << "<int16>: " << boost::get<int16_t>(data);
    break;
  case uint16_type:
    os << "<uint16>: " << boost::get<uint16_t>(data);
    break;
  case int32_type:
    os << "<int32>: " << boost::get<int32_t>(data);
    break;
  case uint32_type:
    os << "<uint32>: " << boost::get<uint32_t>(data);
    break;
  case int64_type:
    os << "<int64>: " << boost::get<int64_t>(data);
    break;
  case uint64_type:
    os << "<uint64>: " << boost::get<uint64_t>(data);
    break;
  case float_type:
    os << "<float>: " << boost::get<float>(data);
    break;
  case double_type:
    os << "<double>: " << boost::get<double>(data);
    break;
  case decimal_type:
    {
      decimal_t d = boost::get<decimal_t>(data);
      os << "<decimal>: mag: " << static_cast<unsigned int>(d.first) << " val: " << d.second;
      break;
    }
  case shortstring_type:
    {
      std::string s = boost::get<std::string>(data);
      os << "<shortstring>: len:" << s.length() << " \"" << s << "\"";
      break;
    }
  case longstring_type:
    {
      std::string s = boost::get<std::string>(data);
      os << "<longstring>: len:" << s.length() << " \"" << s << "\"";
      break;
    }
  case fieldarray_type:
    {
      field_array_t a = boost::get<field_array_t>(data);
      os << "<fieldarray>: len:" << a.size() << " {";
      for (field_array_t::const_iterator it = a.begin();
          it != a.end(); ++it)
      {
        value_to_string(os, it->second, it->first);
        os << ", ";
      }
      os << "}";
      break;
    }
  case timestamp_type:
    os << "<timestamp>: " << boost::get<uint64_t>(data);
    break;
  case fieldtable_type:
    os << "<fieldtable>: " << boost::get<table>(data).to_string();
    break;
  case void_type:
    os << "<void>";
    break;
  default:
    throw std::runtime_error("Unknown type");
  }
}
} // namespace amqpp
