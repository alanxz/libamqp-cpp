#include "frame.h"
#include "methods.gen.h"

#include <gtest/gtest.h>

#include <sstream>

TEST(frame_tests, read_write)
{
  amqpp::methods::connection::start_ok::ptr_t start_ok;

  amqpp::detail::frame::ptr_t fr = amqpp::detail::frame::create_from_method(0, start_ok);

  std::stringstream ss;

  fr->write(ss);
  
  amqpp::detail::frame::ptr_t ret_fr = amqpp::detail::frame::read_frame(ss);
  std::cout << ss.good();

  amqpp::detail::method::ptr_t start_ok_deser = amqpp::detail::method::read(ret_fr);
}
