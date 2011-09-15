#include "methods.h"
#include "methods.gen.h"

#include <gtest/gtest.h>

#include <sstream>

TEST(method, method_size)
{
  using namespace amqpp;

  methods::connection::start_ok::ptr_t start_ok = methods::connection::start_ok::create();
  start_ok->get_client_properties().insert(table_entry("product", table_entry::field_value_t(std::string("libamqp-cpp")), table_entry::shortstring_type));
  start_ok->get_client_properties().insert(table_entry("version", table_entry::field_value_t(std::string("v1.0b")), table_entry::shortstring_type));
  start_ok->get_client_properties().insert(table_entry("platform", table_entry::field_value_t(std::string("c++")), table_entry::shortstring_type));
  start_ok->get_client_properties().insert(table_entry("copyright", table_entry::field_value_t(std::string("Alan Antonuk (c) 2011")), table_entry::shortstring_type));
  start_ok->get_client_properties().insert(table_entry("information", table_entry::field_value_t(std::string("http://github.com/alanxz/libamqp-cpp")), table_entry::shortstring_type));

  start_ok->set_mechanism("PLAIN");
  start_ok->set_response("guest guest");
  start_ok->set_locale("en_US");
  std::ostringstream ss;

  start_ok->write(ss);

  EXPECT_EQ(ss.str().length(), start_ok->get_serialized_size());
}