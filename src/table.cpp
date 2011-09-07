#include "table.h"

#include <boost/mpl/at.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

namespace amqpp
{

table_entry::table_entry(const std::string& key, const field_value_t& data, field_type data_type) :
  m_key(key), m_data(data), m_type(data_type)
{
  validate_key_name(m_key);
  validate_data_type(m_data, m_type);
  
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

    template <>
    bool operator()(const std::string&) const
    {
      if (table_entry::shortstring_type == m_type ||
          table_entry::longstring_type == m_type)
        return true;
      else
        return false;
    }

    template <>
    bool operator()(const int64_t&) const
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

} // namespace amqpp