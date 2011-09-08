#include "table.h"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(table_entry, create_uint8)
{
  uint8_t value = 1;
  amqpp::table_entry entry("uint8", amqpp::table_entry::field_value_t(value), amqpp::table_entry::uint8_type);
}

TEST(table_entry, create_int8)
{
  int8_t value = 1;
  amqpp::table_entry entry("int8", amqpp::table_entry::field_value_t(value), amqpp::table_entry::int8_type);
}

TEST(table_entry, create_int16)
{
  int8_t value = 1;
  amqpp::table_entry entry("int8", amqpp::table_entry::field_value_t(value), amqpp::table_entry::int8_type);
}

TEST(table_entry, create_short_string)
{
  std::string value("abc");
  amqpp::table_entry entry("str", amqpp::table_entry::field_value_t(value), amqpp::table_entry::shortstring_type);
}

TEST(table_entry, create_long_string)
{
  std::string value("abc");
  amqpp::table_entry entry("str", amqpp::table_entry::field_value_t(value), amqpp::table_entry::longstring_type);
}

TEST(table_entry, key_too_long)
{
  int8_t value = 1;
  std::string key(129, 'a');
  EXPECT_THROW(amqpp::table_entry entry(key, amqpp::table_entry::field_value_t(value), amqpp::table_entry::int8_type),
        std::runtime_error);
}

TEST(table_entry, incorrect_datatype)
{
  int8_t value = 1;
  EXPECT_THROW(amqpp::table_entry entry("wrongdata", amqpp::table_entry::field_value_t(value), amqpp::table_entry::fieldarray_type),
               std::runtime_error);
}

