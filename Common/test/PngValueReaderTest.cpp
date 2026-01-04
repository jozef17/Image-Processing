#include <gtest/gtest.h>
#include <vector>
#include <utility>
#include <cstdint>

#include "png/PngValueReader.hpp"

TEST(PngValueReader, Read1bit)
{
	std::vector<uint8_t> data = { 0b00110110, 0b11110010 };
	PngValueReader reader(std::move(data), 1);

	uint16_t values[] = { 0,1,1,0,1,1,0,0, 0,1,0,0,1,1,1,1 };
	for (int i = 0; i < 16; i++)
	{
		EXPECT_EQ(reader.GetNextValue(), values[i]);
	}
}

TEST(PngValueReader, Read2bits)
{
	std::vector<uint8_t> data = { 0b00110110, 0b11110010 };
	PngValueReader reader(std::move(data), 2);

	uint16_t values[] = { 2,1,3,0, 2,0,3,3 };
	for (int i = 0; i < 8; i++)
	{
		EXPECT_EQ(reader.GetNextValue(), values[i]);
	}
}

TEST(PngValueReader, Read4bits)
{
	std::vector<uint8_t> data;
	for (int i = 0; i <= 0x0f; i += 2)
	{
		data.push_back((i+1) << 4 | i);
	}

	PngValueReader reader(std::move(data), 4);

	for (int i = 0; i <= 0x0f; i++)
	{
		EXPECT_EQ(reader.GetNextValue(), i);
	}
}