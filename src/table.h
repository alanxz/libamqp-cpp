#ifndef _AMQPP_TABLE_H_INCLUDED_
#define _AMQPP_TABLE_H_INCLUDED_

#include "export.h"

#include <boost/cstdint.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

// This is here to stop errors from cropping up in MSVC 10
#ifdef _MSC_VER
namespace boost
{
  struct recursive_variant_ {};
}
#endif // _MSC_VER

namespace amqpp
{

class table_entry;


class AMQPP_EXPORT table
{
public:
  typedef std::map<const std::string, table_entry> table_impl_t;

  void insert(const table_entry& e);

  table_impl_t& get_map() { return m_table; }
  const table_impl_t& get_map() const { return m_table; }

private:
  table_impl_t m_table;
};

class AMQPP_EXPORT table_entry
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

  // Hacks:

  /*
   * Decimal type, first is the magnitude, second is the value.
   * How you deal with this is up to you
   */
  typedef std::pair<uint8_t, int32_t> decimal_t;

  /*
   * Void type
   * Doesn't contain any value - for a void element in a table
   */
  typedef struct { } void_t;

  /*
   * Array object
   * Use the field_array_t typedef for an array object
   *
   * This is here because the boost::recursive_variant_ doesn't work
   * if you nest it in a template
   */
  template <class FirstT, class SecondT>
  struct vector_of_pairs
  {
    typedef std::vector<std::pair<FirstT, SecondT> > type;
  };

  /*
   * Defines a field value which can be one of a number of types
   */
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
    vector_of_pairs<boost::recursive_variant_, field_type>::type,
    table,
    void_t
  >::type field_value_t;

  typedef vector_of_pairs<field_value_t, field_type>::type field_array_t;


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
