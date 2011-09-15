#include "frame.h"
#include "methods.gen.h"

#include <gtest/gtest.h>

#include <sstream>

TEST(frame_tests, read_write)
{
  amqpp::methods::connection::start_ok start_ok;

  amqpp::detail::frame fr(0, start_ok);

  std::stringstream ss;

  fr.write(ss);
  
  boost::shared_ptr<amqpp::detail::frame> ret_fr = amqpp::detail::frame::read_frame(ss);
  std::cout << ss.good();

  boost::shared_ptr<amqpp::detail::method> start_ok_deser = amqpp::detail::method::read(*ret_fr);
}