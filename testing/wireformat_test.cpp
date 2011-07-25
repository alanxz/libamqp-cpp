
#include "wireformat.h"
#include "byteswap.h"

#include <gtest/gtest.h>
#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>

#include <sstream>
#include <string>
#include <stdexcept>


TEST(wireformat, write_uint8)
{
	std::ostringstream os;
	uint8_t value = 0x30; // ASCII for 0

	amqpp::detail::wireformat::write_uint8(os, value);

	std::string serialized = os.str();

	EXPECT_EQ(value, serialized[0]);
}

TEST(wireformat, write_uint16)
{
	std::ostringstream os;
	uint16_t value = 0x3031;

	amqpp::detail::wireformat::write_uint16(os, value);

	std::string serialized = os.str();

	EXPECT_EQ(0x30, serialized[0]);
	EXPECT_EQ(0x31, serialized[1]);
}

TEST(wireformat, write_uint32)
{
	std::ostringstream os;
	uint32_t value = 0x30313233;

	amqpp::detail::wireformat::write_uint32(os, value);

	std::string serialized = os.str();

	EXPECT_EQ(0x30, serialized[0]);
	EXPECT_EQ(0x31, serialized[1]);
	EXPECT_EQ(0x32, serialized[2]);
	EXPECT_EQ(0x33, serialized[3]);
}

TEST(wireformat, write_uint64)
{
	std::ostringstream os;
	uint64_t value = 0x3031323334353637;

	amqpp::detail::wireformat::write_uint64(os, value);

	std::string serialized = os.str();

	EXPECT_EQ(0x30, serialized[0]);
	EXPECT_EQ(0x31, serialized[1]);
	EXPECT_EQ(0x32, serialized[2]);
	EXPECT_EQ(0x33, serialized[3]);
	EXPECT_EQ(0x34, serialized[4]);
	EXPECT_EQ(0x35, serialized[5]);
	EXPECT_EQ(0x36, serialized[6]);
	EXPECT_EQ(0x37, serialized[7]);
}

TEST(wireformat, write_shortstring)
{
	std::ostringstream os;
	std::string output("This is 10");
	amqpp::detail::wireformat::write_shortstring(os, output);

	std::string serialized = os.str();
	EXPECT_EQ(output.length(), serialized[0]);
	EXPECT_EQ(output, serialized.substr(1));
}

TEST(wireformat, write_shortstringlong)
{
	std::ostringstream os;
	std::string output(255, 'n');
	amqpp::detail::wireformat::write_shortstring(os, output);

	std::string serialized = os.str();
	EXPECT_EQ(static_cast<char>(output.length()), serialized[0]);
	EXPECT_EQ(output, serialized.substr(1));
}

TEST(wireformat, write_shortstringtoolong)
{
	std::ostringstream os;
	std::string output(256, 'n');
	EXPECT_THROW(amqpp::detail::wireformat::write_shortstring(os, output), std::logic_error);
}

TEST(wireformat, write_longstring)
{
	std::stringstream os;
	std::string output("This is a test");
	amqpp::detail::wireformat::write_longstring(os, output);

	uint32_t len;
	os.read(reinterpret_cast<char*>(&len), 4);
	len = amqpp::detail::byteswap(len);

	std::string serialized = os.str();

	EXPECT_EQ(output.length(), len);
	EXPECT_EQ(output, serialized.substr(4));
}

TEST(wireformat, read_uint8)
{
	std::istringstream is("\x30");
	uint8_t value = 0;
	amqpp::detail::wireformat::read_uint8(is, value);

	EXPECT_EQ(0x30, value);
}

TEST(wireformat, read_uint16)
{
	std::istringstream is("\x30\x31");
	uint16_t value = 0;
	amqpp::detail::wireformat::read_uint16(is, value);

	EXPECT_EQ(0x3031, value);
}

TEST(wireformat, read_uint32)
{
	std::istringstream is("\x30\x31\x32\x33");
	uint32_t value = 0;
	amqpp::detail::wireformat::read_uint32(is, value);

	EXPECT_EQ(0x30313233, value);
}

TEST(wireformat, read_uint64)
{
	std::istringstream is("\x30\x31\x32\x33\x34\x35\x36\x37");
	uint64_t value = 0;
	amqpp::detail::wireformat::read_uint64(is, value);

	EXPECT_EQ(0x3031323334353637, value);
}

TEST(wireformat, read_shortstring)
{
	std::istringstream is("\x0A 123456789");
	std::string value;
	amqpp::detail::wireformat::read_shortstring(is, value);

	EXPECT_EQ(std::string(" 123456789"), value);
}

TEST(wireformat, read_shortstringfail)
{
	std::istringstream is("\x0A 12345678");
	std::string value;
	EXPECT_THROW(amqpp::detail::wireformat::read_shortstring(is, value), std::runtime_error);
}

TEST(wireformat, read_longstring)
{
	std::string input("\x00\x00\x00\x0a 123456789", 14);
	std::istringstream is(input);
	std::string value;
	amqpp::detail::wireformat::read_longstring(is, value);

	EXPECT_EQ(std::string(" 123456789"), value);
}

TEST(wireformat, read_longstringfail)
{
	std::string input("\x00\x00\x00\x0b 123456789", 14);
	std::istringstream is(input);
	std::string value;
	EXPECT_THROW(amqpp::detail::wireformat::read_longstring(is, value), std::runtime_error);
}
