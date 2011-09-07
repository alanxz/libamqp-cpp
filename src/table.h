#ifndef _AMQPP_TABLE_H_INCLUDED_
#define _AMQPP_TABLE_H_INCLUDED_

#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace boost::mpl;

#ifdef BOOST_VARIANT_NO_FULL_RECURSIVE_VARIANT_SUPPORT
# err nooo
#endif
namespace amqpp
{

class table_entry;

class table
{
public:
  typedef std::map<const std::string, table_entry> table_impl_t;

  void insert(const table_entry& e);

  table_impl_t& get_map();
  const table_impl_t& get_map() const;

private:
  table_impl_t m_table;
};

class table_entry
{
public:
  enum field_type
  {
    boolean_type = 't',
    int8_type = 'b',
    uint8_type = 'B',
    int16_type = 'U',
    uint16_type = 'u',
    int32_type = 'I',
    uint32_type = 'i',
    int64_type = 'L',
    uint64_type = 'l',
    float_type = 'f',
    double_type = 'd',
    decimal_type = 'D',
    shortstring_type = 's',
    longstring_type = 'S',
    fieldarray_type = 'A',
    timestamp_type = 'T',
    fieldtable_type = 'F',
    void_type = 'V'
  };

  typedef std::pair<uint8_t, uint32_t> decimal_t;
  typedef struct { } void_t;

  typedef boost::make_recursive_variant<
    bool,
    int8_t,
    uint8_t,
    int16_t,
    uint16_t,
    int32_t,
    uint32_t,
    int64_t,
    uint64_t,
    float,
    double,
    decimal_t,
    std::string,
    std::vector<boost::recursive_variant_>,
    table,
    void_t
  >::type field_value_t;

  typedef std::vector<std::pair<field_type, field_value_t> > field_array_t;

  //typedef boost::mpl::map<
  //  boost::mpl::pair<boost::mpl::int_<boolean_type>,    bool>,
  //  boost::mpl::pair<boost::mpl::int_<int8_type>,       int8_t>,
  //  boost::mpl::pair<boost::mpl::int_<uint8_type>,      uint8_t>,
  //  boost::mpl::pair<boost::mpl::int_<int16_type>,      int16_t>,
  //  boost::mpl::pair<boost::mpl::int_<uint16_type>,     uint16_t>,
  //  boost::mpl::pair<boost::mpl::int_<int32_type>,      int32_t>,
  //  boost::mpl::pair<boost::mpl::int_<uint32_type>,     uint32_t>,
  //  boost::mpl::pair<boost::mpl::int_<int64_type>,      int64_t>,
  //  boost::mpl::pair<boost::mpl::int_<float_type>,      float>,
  //  boost::mpl::pair<boost::mpl::int_<double_type>,     double>,
  //  boost::mpl::pair<boost::mpl::int_<decimal_type>,    decimal_t>,
  //  boost::mpl::pair<boost::mpl::int_<shortstring_type>,std::string>,
  //  boost::mpl::pair<boost::mpl::int_<longstring_type>, std::string>,
  //  boost::mpl::pair<boost::mpl::int_<fieldarray_type>, field_array_t>,
  //  boost::mpl::pair<boost::mpl::int_<timestamp_type>,  int64_t>,
  //  boost::mpl::pair<boost::mpl::int_<fieldtable_type>, table>,
  //  boost::mpl::pair<boost::mpl::int_<void_type>,       int8_t>
  //> field_type_to_field_value_t_map;


  explicit table_entry(const std::string& key, const field_value_t& data, field_type data_type);
  virtual ~table_entry();

  // 
  inline bool operator<(const table_entry& r) const { return m_key < r.m_key; }

  inline const std::string& get_key() const { return m_key; }
  inline const field_value_t& get_data() const { return m_data; }
  inline const field_type get_type() const { return m_type; }

  static bool validate_key_name(const std::string& key);
  static bool validate_data_type(const field_value_t& data, field_type type);
private:
  std::string m_key;
  field_value_t m_data;
  field_type m_type;
};

} // namespace amqpp
#endif // _AMQPP_TABLE_H_INCLUDED_