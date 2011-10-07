#include "detail/byteswap.h"

#include <gtest/gtest.h>
#include <boost/cstdint.hpp>

TEST(byteswap, byteswap16)
{
	uint16_t v = 0x4142; // A, B
	union {
		uint16_t i;
		uint8_t b[2];
	} ret;

   ret.i = amqpp::detail::byteswap(v);

	EXPECT_EQ(0x41, ret.b[0]);
	EXPECT_EQ(0x42, ret.b[1]);
}

TEST(byteswap, byteswap32)
{
	uint32_t v = 0x41424344;
	union {
		uint32_t i;
		uint8_t b[4];
	} ret;
	ret.i = amqpp::detail::byteswap(v);

	EXPECT_EQ(0x41, ret.b[0]);
	EXPECT_EQ(0x42, ret.b[1]);
	EXPECT_EQ(0x43, ret.b[2]);
	EXPECT_EQ(0x44, ret.b[3]);

}

TEST(byteswap, byteswap64)
{
	uint64_t v = 0x4142434445464748;
	union { 
		uint64_t i;
		uint8_t b[8];
	} ret;
	ret.i = amqpp::detail::byteswap(v);

	EXPECT_EQ(0x41, ret.b[0]);
	EXPECT_EQ(0x42, ret.b[1]);
	EXPECT_EQ(0x43, ret.b[2]);
	EXPECT_EQ(0x44, ret.b[3]);
	EXPECT_EQ(0x45, ret.b[4]);
	EXPECT_EQ(0x46, ret.b[5]);
	EXPECT_EQ(0x47, ret.b[6]);
	EXPECT_EQ(0x48, ret.b[7]);
}

TEST(byteswap, sanity)
{
	uint32_t v = 0x41424344;
	uint32_t bv = amqpp::detail::byteswap(v);

	EXPECT_NE(v, bv);
}

