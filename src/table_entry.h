#ifndef AMQPP_TABLE_ENTRY_H
#define AMQPP_TABLE_ENTRY_H

#include "export.h"
#include "table.h"

#include <boost/cstdint.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

// This is here to stop errors from cropping up in MSVC 10
#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4251 )
namespace boost
{
  struct recursive_variant_ {};
}
#endif // _MSC_VER
namespace amqpp {

class AMQPP_EXPORT table_entry
{
  /** Implementation notes, this does not follow the spec, it follows the rabbitmq errata
    * http://dev.rabbitmq.com/wiki/Amqp091Errata#section_3
    */
public:
  enum field_type
  {
    boolean_type = 't',
    int8_type = 'b',
    int16_type = 'U',
    int32_type = 'I',
    int64_type = 'L',
    float_type = 'f',
    double_type = 'd',
    decimal_type = 'D',
    longstring_type = 'S',
    fieldarray_type = 'A',
    timestamp_type = 'T',
    fieldtable_type = 'F',
    void_type = 'V',
    bytes_type = 'x'
  };

  // Hacks:

  /*
   * Decimal type, first is the magnitude, second is the value.
   * How you deal with this is up to you
   */
  typedef std::pair<uint8_t, int32_t> decimal_t;

  typedef uint64_t timestamp_t;

  /*
   * Void type
   * Doesn't contain any value - for a void element in a table
   */
  typedef struct { } void_t;


  typedef std::vector<uint8_t> bytes_t;
  /*
   * Defines a field value which can be one of a number of types
   */
  typedef boost::make_recursive_variant<
    bool,
    int8_t,
    int16_t,
    int32_t,
    int64_t,
    float,
    double,
    decimal_t,
    std::string,
    std::vector<boost::recursive_variant_>,
    timestamp_t,
    table,
    void_t,
    bytes_t
  >::type field_value_t;

  typedef std::vector<field_value_t> array_t;


  explicit table_entry(const std::string& key, const field_value_t& data);
  virtual ~table_entry();

  inline bool operator<(const table_entry& r) const { return m_key < r.m_key; }

  inline const std::string& get_key() const { return m_key; }
  inline const field_value_t& get_data() const { return m_data; }
  field_type get_type() const;

  static bool validate_key_name(const std::string& key);

  std::string to_string() const;
  static void value_to_string(std::ostream& os, const field_value_t& data);

  uint32_t wireformat_size() const;
  static uint32_t wireformat_data_size(const field_value_t& data);

private:
  std::string m_key;
  field_value_t m_data;
};


} // namespace amqpp

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

#endif // AMQPP_TABLE_ENTRY_H
