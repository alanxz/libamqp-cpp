#include "table.h"
#include "detail/wireformat.h"

#include <gtest/gtest.h>
#include <stdexcept>


TEST(table_entry, create_int8)
{
  int8_t value = 1;
  amqpp::table_entry entry("int8", value);
}

TEST(table_entry, create_int16)
{
  int16_t value = 2;
  amqpp::table_entry entry("int16", value);
}

TEST(table_entry, create_long_string)
{
  amqpp::table_entry entry("str", "random string");
}

TEST(table_entry, key_too_long)
{
  int8_t value = 1;
  std::string key(129, 'a');
  EXPECT_THROW(amqpp::table_entry entry(key, value), std::runtime_error);
}

TEST(table_entry, table_entry_size)
{
  amqpp::table_entry entry("key", "value");
  std::ostringstream os;
  amqpp::detail::wireformat::write_table_entry(os, entry);

  EXPECT_EQ(os.str().length(), entry.wireformat_size());
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

  EXPECT_EQ(ss.str().length(), table_in.wireformat_size());
}

TEST(table, table_size_sstring)
{
  amqpp::table table_in;
  table_in.insert(amqpp::table_entry("key", "Value"));
  std::stringstream ss;
  amqpp::detail::wireformat::write_table(ss, table_in);

  EXPECT_EQ(ss.str().length(), table_in.wireformat_size());
}

TEST(table, table_size_sstring2)
{
  amqpp::table table_in;
  table_in.insert(amqpp::table_entry("key", "value"));
  table_in.insert(amqpp::table_entry("key2", "value2"));
  std::stringstream ss;
  amqpp::detail::wireformat::write_table(ss, table_in);

  EXPECT_EQ(ss.str().length(), table_in.wireformat_size());
}

TEST(table, table_read_write)
{
  using namespace amqpp;

  table table_in;
  table_in.insert(table_entry("product", "libamqp-cpp"));
  table_in.insert(table_entry("version", "0.1b"));
  table_in.insert(table_entry("platform", "c++"));
  table_in.insert(table_entry("copyright", "Alan Antonuk (c) 2011"));
  table_in.insert(table_entry("information", "http://github.com/alanxz/libamqp-cpp"));

  std::stringstream ss;
  detail::wireformat::write_table(ss, table_in);
  table table_out = detail::wireformat::read_table(ss);

  EXPECT_EQ(table_in.to_string(), table_out.to_string());
}
