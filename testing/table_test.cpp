#include "table.h"
#include "wireformat.h"

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

TEST(table_entry, table_entry_size)
{
  amqpp::table_entry entry("key", amqpp::table_entry::field_value_t(std::string("value")), amqpp::table_entry::shortstring_type);
  std::ostringstream os;
  amqpp::detail::wireformat::write_table_entry(os, entry);

  EXPECT_EQ(os.str().length(), entry.serialized_size());
}

TEST(table, table_read_write_empty)
{
  amqpp::table table_in;
  std::stringstream ss;

  amqpp::detail::wireformat::write_table(ss, table_in);
  amqpp::table table_out = amqpp::detail::wireformat::read_table(ss);
}

TEST(table, table_size)
{
  amqpp::table table_in;
  std::stringstream ss;
  amqpp::detail::wireformat::write_table(ss, table_in);

  EXPECT_EQ(ss.str().length(), table_in.serialized_size());
}

TEST(table, table_size_sstring)
{
  amqpp::table table_in;
  table_in.insert(amqpp::table_entry("key", amqpp::table_entry::field_value_t(std::string("Value")), amqpp::table_entry::shortstring_type));
  std::stringstream ss;
  amqpp::detail::wireformat::write_table(ss, table_in);

  EXPECT_EQ(ss.str().length(), table_in.serialized_size());
}

TEST(table, table_size_sstring2)
{
  amqpp::table table_in;
  table_in.insert(amqpp::table_entry("key", amqpp::table_entry::field_value_t(std::string("Value")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("key2", amqpp::table_entry::field_value_t(std::string("Value2")), amqpp::table_entry::shortstring_type));
  std::stringstream ss;
  amqpp::detail::wireformat::write_table(ss, table_in);

  EXPECT_EQ(ss.str().length(), table_in.serialized_size());
}

TEST(table, table_size_sstring4)
{
  amqpp::table table_in;
  table_in.insert(amqpp::table_entry("product", amqpp::table_entry::field_value_t(std::string("libamqp-cpp")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("version", amqpp::table_entry::field_value_t(std::string("v1.0b")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("platform", amqpp::table_entry::field_value_t(std::string("c++")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("copyright", amqpp::table_entry::field_value_t(std::string("Alan Antonuk (c) 2011")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("information", amqpp::table_entry::field_value_t(std::string("http://github.com/alanxz/libamqp-cpp")), amqpp::table_entry::shortstring_type));

  std::ostringstream ss;
  amqpp::detail::wireformat::write_table(ss, table_in);

  EXPECT_EQ(ss.str().length(), table_in.serialized_size());
}

TEST(table, table_read_write)
{
  amqpp::table table_in;
  table_in.insert(amqpp::table_entry("product", amqpp::table_entry::field_value_t(std::string("libamqp-cpp")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("version", amqpp::table_entry::field_value_t(std::string("v1.0b")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("platform", amqpp::table_entry::field_value_t(std::string("c++")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("copyright", amqpp::table_entry::field_value_t(std::string("Alan Antonuk (c) 2011")), amqpp::table_entry::shortstring_type));
  table_in.insert(amqpp::table_entry("information", amqpp::table_entry::field_value_t(std::string("http://github.com/alanxz/libamqp-cpp")), amqpp::table_entry::shortstring_type));

  std::stringstream ss;
  amqpp::detail::wireformat::write_table(ss, table_in);
  amqpp::table table_out = amqpp::detail::wireformat::read_table(ss);

  EXPECT_EQ(table_in.to_string(), table_out.to_string());
}