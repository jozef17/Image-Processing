#include <gtest/gtest.h>
#include <memory>

#include "png/Type10Decoder.hpp"
#include "BitStream.hpp"

// NOTE: Test created by getting bit stream from png image and manually decoded
TEST(Type10DecoderTest, Decode1)
{
#define SIZE 254
	// Input stream
	uint8_t reference[SIZE] =
	{ //  Header               Code Lengths
	  //  HLIT  HDIST    HCLEN 16      17  18  0       8   7      9   6   10      5   11  4       12  3
		0b10111'100, 0b01'0111'00, 0b0'110'010'0, 0b00'000'000, 0b000'000'00, 0b0'010'000'1, 0b10'000'000,
      //                      Literal/length codes
      //  13  2   14      1   4   5      5  18 125x0       5  18    126x0   5      5  17  4x0    5  18 19x0
        0b000'110'00, 0b0'110'110'0, 0b0'00'01'010, 0b0111'00'01, 0b1100111'0, 0b0'00'111'10, 0b000'01'000,
	  //                          Distance codes                            DATA
      //	   2   17      3x0 1      1       17    4x0  2  18    11x0  2   
		0b1000'101'1, 0b11'000'100, 0b100'111'10, 0b0101'01'00, 0b00000'101, 0b11010111, 0b01111001,           
		0b10011101, 0b11000010, 0b01100011, 0b01111111, 0b10100100, 0b01000011, 0b00001001, 0b00011000,
		0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001,
		0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011,
		0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100,
		0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100,
		0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001,
		0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000,
		0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110,
		0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010,
		0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000,
		0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001,
		0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011,
		0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100,
		0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100,
		0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001,
		0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 
		0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 
		0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010,
		0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000,
		0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001,
		0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011,
		0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100,
		0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001, 0b10001100,
		0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000, 0b10010001,
		0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110, 0b00110000,
		0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010, 0b01000110,
		0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000, 0b11000010,
		0b01000110, 0b00110000, 0b10010001, 0b10001100, 0b00100100, 0b01100011, 0b00001001, 0b00011000,
		0b11000010, 0b01001010, 0b01100110, 0b01111000, 0b00000100, 0b00010101, 0b10111110, 0b00000011,
		0b00010'000
	};

	std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[SIZE]);
	for (int i = 0; i < SIZE; i++)
	{
		auto num = reference[i];
		// change bit order
		auto bits = 0;
		for (int j = 0; j < 8; j++)
		{
			auto bit = (num & 1 << j) >> j;
			bits = bits | bit << (7 - j);
		}
		data[i] = bits;
	}

	// Decode
	BitStream stream;
	stream.Append(std::move(data), SIZE);
	Type10Decoder decoder(stream);
	std::vector<uint8_t> result;
	decoder.Decode(result);

	// Check result
	EXPECT_EQ(result.size(), 40100); // Size
	
	// Check first line
	EXPECT_EQ(result.at(0), 1);   // 1
	EXPECT_EQ(result.at(1), 255); // 255
	EXPECT_EQ(result.at(2), 128); // 128
	EXPECT_EQ(result.at(3), 0);   // 0
	EXPECT_EQ(result.at(4), 255); // 255
	EXPECT_EQ(result.at(5), 0);   //0
	for (int i = 6; i < 401; i++)
	{
		EXPECT_EQ(result.at(i), 0);
	}

	// Second line
	EXPECT_EQ(result.at(401), 2);   //2
	for (int i = 0; i < 400; i++)
	{
		EXPECT_EQ(result.at(i+402), 0);
	}

	// Third line
	EXPECT_EQ(result.at(802), 2);   //2
	for (int i = 0; i < 400; i++)
	{
		EXPECT_EQ(result.at(i + 803), 0);
	}
}
