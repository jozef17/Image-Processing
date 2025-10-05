#include <gtest/gtest.h>
#include <memory>

#include "png/Type01Decoder.hpp"
#include "BitStream.hpp"

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

	std::vector<uint8_t> result;
	Type01Decoder decoder(stream);
	decoder.Decode(result);

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

	std::vector<uint8_t> result;
	Type01Decoder decoder(stream);
	decoder.Decode(result);

	// Check result
	for (int i = 0; i < result.size(); i++)
	{
		EXPECT_EQ(result.at(i), i + 144);
	}
}

// NOTE: Test created by getting bit stream from png image and manually decoded
TEST(Type01DecoderTest, Decode1)
{
#define SIZE 121
	// Input stream
	uint8_t reference[SIZE] =
	{ //  0           257-L3  D1        255           258-L4      D1        181           230           29           255
		0b00110000, 0b0000001'0, 0b0000'1111, 0b11111'000, 0b0010'0000, 0b0'1101101, 0b01'111100, 0b110'01001, 0b101'11111,
	  //       255           261-L7      D12       EX 258-L4      D20   EX      2            237           28           36
		0b1111'1111, 0b11111'000, 0b0101'0011, 0b0'11'00000, 0b10'01000'1, 0b10'001100, 0b10'111101, 0b101'01001, 0b100'01010,
	  //      0            64           73           205           0            19           217           202           257-L3
		0b100'00110, 0b000'01110, 0b000'01111, 0b001'11100, 0b1101'0011, 0b0000'0100, 0b0011'1110, 0b11001'111, 0b001010'00,
      //        D15       EX 0            0            1            1            1            258-L4      D30      EX  0
		0b00001'001, 0b11'01'0011, 0b0000'0011, 0b0000'0011, 0b0001'0011, 0b0001'0011, 0b0001'0000, 0b010'01001, 0b101'00110,
	  //      0            34           177           76           255           255           242           0            255
		0b000'00110, 0b000'01010, 0b010'11011, 0b0001'0111, 0b1100'1111, 0b11111'111, 0b111111'11, 0b1110010'0, 0b0110000'1,
	  //              126         250           133          255           185           122          87           264-L10
		0b11111111, 0b10101110, 0b11111101, 0b0'1011010, 0b1'1111111, 0b11'110111, 0b001'10101, 0b010'10000, 0b111'00010,
	  //     D50   EX       150           192           4            0            182           244           29           0
		0b00'01011'1, 0b000'11001, 0b0110'1110, 0b00000'001, 0b10100'001, 0b10000'110, 0b110110'11, 0b1110100'0, 0b1001101'0,
	  //          26           126          16           0            10           73           108          264-L10     D50
		0b0110000'0, 0b1001010'1, 0b0101110'0, 0b1000000'0, 0b0110000'0, 0b0111010'0, 0b1111001'1, 0b0011100'0, 0b001000'01,
	  //      EX   63           72           204              255           112          146           190           255
		0b011'1000'0, 0b1101111'0, 0b1111000'1, 0b11001100, 0b11111111, 0b1'1010000, 0b0'1100100, 0b10'110111, 0b110'11111,
	  //       255           201           14           258-L4      D16       EX 264-L10     D50      EX   121          41
		0b1111'1111, 0b11111'111, 0b001001'00, 0b111110'00, 0b00010'001, 0b11'11'0001, 0b000'01011, 0b1000'1010, 0b1001'0101,
	  //       132          0            207           182           14           0            182           29           15
		0b1001'1011, 0b0100'0011, 0b0000'1110, 0b01111'110, 0b110110'00, 0b111110'00, 0b110000'11, 0b0110110'0, 0b1001101'0,
	  //          257-L3      D97       EX       265-L11 LX   D100  EX        258-L4      D96   EX        258-L4      D1    258-L4
		0b0111111'0, 0b000001'01, 0b101'00000, 0b0001001'0, 0b01101'110, 0b00'000001, 0b0'01100'11, 0b111'00000, 0b10'00000'0,
      //         D96       EX       265-L12 LX   D100  EX        257-L3      D4    264-L10     D63       EX   264-L10     D63
		0b000010'01, 0b100'11111, 0b0001001'1, 0b01101'110, 0b00'000000, 0b1'00011'00, 0b01000'010, 0b11'0111'00, 0b01000'001,
	  //     EX255           255           256-EOB
		0b01'1'11111, 0b1111'1111, 0b11111'000, 0b0000'0000 // 111011010011100010011000100110 // TODO CRC
	};

	// Expected values
	uint8_t expected[200] =
	{   0,      0,   0,   0, 255,    255, 255, 255, 255,    181, 230,  29, 255,    255,   0,   0, 255,    255, 255, 255, 255,      0,   0,   0, 255,
		2,    237,  28,  36,   0,     64,  73, 205,   0,     19, 217, 202,   0,      0, 255,   0,   0,      1,   1,   1,   0,    255, 255, 255,   0,
		0,     34, 177,  76, 255,    255, 242,   0, 255,    126, 250, 133, 255,    185, 122,  87, 255,    255, 255, 255, 255,      0,   0,   0, 255,
		2,    150, 192,   4,   0,    182, 244,  29,   0,     26, 126,  16,   0,     10,  73, 108,   0,      1,   1,   1,   0,    255, 255, 255,   0,
		0,     63,  72, 204, 255,    112, 146, 190, 255,    255, 201,  14, 255,    255, 255,   0, 255,    255, 255, 255, 255,      0,   0,   0, 255,
		2,    121,  41, 132,   0,    207, 182,  14,   0,    182,  29,  15,   0,      0,   1,   0,   0,      1,   1,   1,   0,    255, 255, 255,   0,
		0,    255, 242,   0, 255,    255, 255, 255, 255,    185, 122,  87, 255,    185, 122,  87, 255,    255, 255, 255, 255,      0,   0,   0, 255,
		0,      0,   0,   0, 255,    255, 255, 255, 255,      0,   0,   0, 255,    255, 255, 255, 255,      0,   0,   0, 255,    255, 255, 255, 255 };

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

	std::vector<uint8_t> result;
	Type01Decoder decoder(stream);
	decoder.Decode(result);

	// Check result
	EXPECT_EQ(result.size(), 200);
	for (int i = 0; i < result.size(); i++)
	{
		EXPECT_EQ(result.at(i), expected[i]);
	}
}

