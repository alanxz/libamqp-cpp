#include "frame.h"
#include "frame_builder.h"
#include "frame_writer.h"
#include "methods.gen.h"

#include <boost/asio/buffers_iterator.hpp>
#include <gtest/gtest.h>

#include <sstream>

using namespace boost::asio;
using namespace amqpp::detail;
using namespace amqpp::methods;

TEST(frame_tests, read_write)
{
  connection::start_ok::ptr_t start_ok = connection::start_ok::create();
  frame::ptr_t fr = frame::create_from_method(0, start_ok);

  frame_writer writer;
  frame_builder builder;

  frame_writer::buffer_sequence_t out = writer.get_sequence(fr);

  frame_builder::buffer_t in1 = builder.get_header_buffer();

  buffers_iterator<frame_writer::buffer_sequence_t> it_out(boost::asio::buffers_begin(out));
  buffers_iterator<frame_writer::buffer_sequence_t> out_end(boost::asio::buffers_end(out));

  buffers_iterator<frame_builder::buffer_t> it_in(boost::asio::buffers_begin(in1));
  buffers_iterator<frame_builder::buffer_t> in_end(boost::asio::buffers_end(in1));

  for (; it_in != in_end && it_out != out_end; ++it_in, ++it_out)
  {
    *it_in = *it_out;
  }

  frame_builder::buffer_sequence_t in2 = builder.get_body_buffer();
  buffers_iterator<frame_builder::buffer_sequence_t> it_in2(boost::asio::buffers_begin(in2));
  buffers_iterator<frame_builder::buffer_sequence_t> in_end2(boost::asio::buffers_end(in2));

  for (; it_in != in_end && it_out != out_end; ++it_in, ++it_out)
  {
    *it_in = *it_out;
  }

  frame::ptr_t fr2 = builder.create_frame();
}
