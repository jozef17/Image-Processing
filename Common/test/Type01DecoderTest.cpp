#include <gtest/gtest.h>
#include <memory>

#include "png/Type01Decoder.hpp"
#include "png/BitStream.hpp"

TEST(Type01DecoderTest, Literals0to143)
{
	std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[145]);
	// Init bit stream with literals 0-143
	for (int i = 0; i < 144; i++)
	{
		auto num = 0b00110000 + i;
		// change bit order
		auto bits = 0;
		for (int j = 0; j < 8; j++)
		{
			auto bit = (num & 1 << j) >> j;
			bits = bits | bit << (7 - j);
		}
		data[i] = bits;
	}
	data[144] = 0; // End of block marker

	// Decode
	BitStream stream;
	stream.Append(std::move(data), 145);
	Type01Decoder decoder(stream);
	auto result = decoder.Decode();

	// Check result
	for (int i = 0; i < 144; i++)
	{
		EXPECT_EQ(result.at(i), i);
	}
}

TEST(Type01DecoderTest, Literals144to255)
{
	std::unique_ptr<uint8_t[]> data = std::unique_ptr<uint8_t[]>(new uint8_t[127]);

	// Init bit stream with literals 144-255
	auto bitLoc = 0;
	auto byteLoc = 0;
	data[byteLoc] = 0;
	for (uint16_t i = 0; i < 112; i++)
	{
		uint16_t num = 0b110010000 + i;
		for (int j = 8; j >= 0; j--)
		{
			uint16_t bit = (num & ((uint16_t)1 << j)) >> j;
			data[byteLoc] = data[byteLoc] | (bit << bitLoc++);
			if (bitLoc == 8)
			{
				bitLoc = 0;
				byteLoc++;
				data[byteLoc] = 0;
			}
		}
	}

	// Decode
	BitStream stream;
	stream.Append(std::move(data), 127);
	Type01Decoder decoder(stream);
	auto result = decoder.Decode();

	// Check result
	for (int i = 0; i < result.size(); i++)
	{
		EXPECT_EQ(result.at(i), i + 144);
	}
}

TEST(Type01DecoderTest, Decode1)
{
#define SIZE 121
	// Input stream
	uint8_t reference[SIZE] = 
		{ 0b00110000, 0b00000010, 0b00001111, 0b11111000, 0b00100000, 0b01101101, 0b01111100, 0b11001001, 0b10111111,
		  0b11111111, 0b11111000, 0b01010011, 0b01100000, 0b10010001, 0b10001100, 0b10111101, 0b10101001, 0b10001010,
		  0b10000110, 0b00001110, 0b00001111, 0b00111100, 0b11010011, 0b00000100, 0b00111110, 0b11001111, 0b00101000, 
          0b00001001, 0b11010011, 0b00000011, 0b00000011, 0b00010011, 0b00010011, 0b00010000, 0b01001001, 0b10100110,
          0b00000110, 0b00001010, 0b01011011, 0b00010111, 0b11001111, 0b11111111, 0b11111111, 0b11100100, 0b01100001,
		  0b11111111, 0b10101110, 0b11111101, 0b01011010, 0b11111111, 0b11110111, 0b00110101, 0b01010000, 0b11100010,
          0b00010111, 0b00011001, 0b01101110, 0b00000001, 0b10100001, 0b10000110, 0b11011011, 0b11101000, 0b10011010,
		  0b01100000, 0b10010101, 0b01011100, 0b10000000, 0b01100000, 0b01110100, 0b11110011, 0b00111000, 0b00100001,
          0b01110000, 0b11011110, 0b11110001, 0b11001100, 0b11111111, 0b11010000, 0b01100100, 0b10110111, 0b11011111,
          0b11111111, 0b11111111, 0b00100100, 0b11111000, 0b00010001, 0b11110001, 0b00001011, 0b10001010, 0b10010101,
          0b10011011, 0b01000011, 0b00001110, 0b01111110, 0b11011000, 0b11111000, 0b11000011, 0b01101100, 0b10011010,
          0b01111110, 0b00000101, 0b10100000, 0b00010010, 0b01101110, 0b00000001, 0b00110011, 0b11100000, 0b10000000,
          0b00001001, 0b10011111, 0b00010011, 0b01101110, 0b00000000, 0b10001100, 0b01000010, 0b11011100, 0b01000001,
          0b01111111, 0b11111111, 0b11111000, 0b00000001
	};

	// Expected values
	uint8_t expected[200] = 
		{   0,   0,   0,   0, 255, 255, 255, 255, 255, 181,
		  230,  29, 255, 255,   0,   0, 255, 255, 255, 255,
		  255,   0,   0,   0,   0,   2, 237,  28,  36,   0,
		   64,  73, 205,   0,  19, 217, 202,   0,   0,   2,
		    0,   0,   1,   1,   1,   0, 255, 255, 255,   0,
		    0,  34, 177,  76, 255, 255, 242,   0, 255, 126,
		  250, 133, 255, 185, 122,  87, 181, 230,  29, 255,
		  255,   0,   0, 255, 255, 255, 150, 192,   4,   0,
		  182, 244,  29,   0,  26, 126,  16,   0,  10,  73,
		  108,  19, 217, 202,   0,   0,   2,   0,   0,   1,
		    1,  63,  72, 204, 255, 112, 146, 190, 255, 255,
		  201,  14,   2,   0,   0,   1, 126, 250, 133, 255,
		  185, 122,  87, 181, 230,  29, 121,  41, 132,   0,
		  207, 182,  14,   0, 182,  29,  15,   0,   0,   1,
		  255, 255,   0,   0,   0,   0,   2, 237,  28,  36,
		    0, 255, 242,   0, 255, 255, 255, 255, 255, 185,
		  122,  87, 181,   1,   1,   1,   0, 255, 255, 255,
		    0,   0,  34, 177,  76,   0,  34, 177,  87, 181,
		  230,  29, 121,  41, 132,   0, 207, 182, 230,  29,
		  121,  41, 132,   0, 207, 182, 230,  29, 255, 255 };

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
	Type01Decoder decoder(stream);
	auto result = decoder.Decode();

	// Check result
	EXPECT_EQ(result.size(), 200);
	for (int i = 0; i < result.size(); i++)
	{
		EXPECT_EQ(result.at(i), expected[i]);
	}
}

