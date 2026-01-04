#include <gtest/gtest.h>

#include "BitStream.hpp"
#include "Exception.hpp"

const uint8_t testData[] = { 0b00000000, 0b11111111, 0b10101010, 0b01010101, 0b11001100, 0b11110000, 0b11001100 };

TEST(BitStreamTest, AppendAndReadBitsMSB)
{
	// Test Data
	auto streamData = std::make_unique<uint8_t[]>(sizeof(testData));
	std::memcpy(streamData.get(), testData, sizeof(testData));

	BitStream bitStream(BitStream::Mode::MSB);
	bitStream.Append(std::move(streamData), sizeof(testData));

	// Check bits
	for (int i = 0; i < sizeof(testData); i++) 
	{
		for (int j = 7; j >= 0; j--)
		{
			bool bit;
			bitStream >> bit;
			bool expectedBit = testData[i] & 1 << j;
			EXPECT_EQ(bit, expectedBit);
		}
	}
}

TEST(BitStreamTest, AppendAndReadBitsLSB)
{
	// Test Data
	auto streamData = std::make_unique<uint8_t[]>(sizeof(testData));
	std::memcpy(streamData.get(), testData, sizeof(testData));

	BitStream bitStream(BitStream::Mode::LSB);
	bitStream.Append(std::move(streamData), sizeof(testData));

	// Check bits
	for (int i = 0; i < sizeof(testData); i++) 
	{
		for (int j = 0; j < 8; j++) 
		{
			bool bit;
			bitStream >> bit;
			bool expectedBit = testData[i] & 1 << j;
			EXPECT_EQ(bit, expectedBit);
		}
	}
}
