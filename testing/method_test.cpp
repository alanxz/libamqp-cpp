#include "methods.h"
#include "methods.gen.h"

#include <gtest/gtest.h>

#include <sstream>

TEST(method, method_size)
{
  using namespace amqpp;

  methods::connection::start_ok::ptr_t start_ok = methods::connection::start_ok::create();
  start_ok->get_client_properties().insert(table_entry("product", "libamqp-cpp"));
  start_ok->get_client_properties().insert(table_entry("version", "0.1b"));
  start_ok->get_client_properties().insert(table_entry("platform", "c++"));
  start_ok->get_client_properties().insert(table_entry("copyright", "Alan Antonuk (c) 2011"));
  start_ok->get_client_properties().insert(table_entry("information", "http://github.com/alanxz/libamqp-cpp"));

  start_ok->set_mechanism("PLAIN");
  start_ok->set_response("guest guest");
  start_ok->set_locale("en_US");
  std::ostringstream ss;

  start_ok->write(ss);

  EXPECT_EQ(ss.str().length(), start_ok->get_serialized_size());
}
