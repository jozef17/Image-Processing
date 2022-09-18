#include <gtest/gtest.h>
#include "DCT.hpp"
#include "Pixel.hpp"



// NOTE: Input and output values for test were generated from MATLAB
TEST(DCTTest, DCTOneBlock)
{
	RGBPixel pixels[8][8] = {
		{{63, 133, 193}, {63, 133, 193}, {65, 133, 194}, {65, 133, 194}, {67, 133, 194}, {65, 133, 194}, {65, 134, 193}, {66, 135, 194}},
		{{61, 131, 191}, {61, 131, 191}, {63, 131, 192}, {64, 132, 193}, {64, 132, 193}, {64, 132, 193}, {65, 133, 194}, {66, 134, 195}},
		{{59, 129, 189}, {60, 130, 190}, {62, 130, 191}, {63, 131, 192}, {63, 131, 192}, {63, 131, 192}, {64, 132, 193}, {64, 132, 193}},
		{{60, 130, 192}, {60, 130, 192}, {62, 130, 193}, {63, 131, 192}, {63, 131, 192}, {62, 130, 191}, {62, 130, 191}, {62, 130, 191}},
		{{62, 130, 193}, {62, 130, 193}, {62, 132, 194}, {62, 132, 192}, {62, 132, 192}, {61, 131, 191}, {61, 131, 191}, {61, 131, 191}},
		{{62, 130, 191}, {62, 130, 191}, {62, 132, 192}, {63, 133, 193}, {62, 132, 192}, {62, 132, 192}, {62, 132, 192}, {63, 133, 193}},
		{{63, 131, 192}, {63, 131, 192}, {62, 132, 192}, {63, 133, 193}, {63, 133, 193}, {63, 133, 193}, {63, 133, 193}, {64, 134, 194}},
		{{63, 131, 192}, {63, 131, 192}, {62, 132, 192}, {63, 133, 193}, {62, 134, 193}, {63, 133, 192}, {64, 134, 193}, {65, 135, 194}}
	};

	float reference[8][8][3] = {
		{{938.750000f, 1320.125000f, 747.750000f}, {-4.504072f,  1.576093f,  0.320364f}, {-0.884251f, -0.067650f, -0.230970f}, {-1.584949f, -0.070152f,  0.112497f}, { 1.500000f, -0.125000f, 0.000000f}, { 0.205932f, -0.197210f, -0.075168f}, {-0.174927f,  0.163320f,  0.095671f}, {-0.184013f,  0.186056f, -0.063724f}},
		{{  0.982749f,    0.173380f,   1.812255f}, {-0.555902f, -0.136224f, -1.973641f}, {-0.289229f, -0.093833f, -2.447733f}, {-0.176177f,  0.240485f, -0.387008f}, {-0.335316f, -0.173380f, 0.346760f}, { 0.027314f, -0.047835f,  0.032887f}, { 0.057531f,  0.226532f,  0.638554f}, { 0.295603f, -0.203873f,  0.854521f}},
		{{  6.499986f,   -1.796524f,  -0.326641f}, {-2.210629f, -0.128320f,  0.418576f}, { 1.332107f, -0.941942f, -0.301777f}, {-0.285877f,  0.226532f,  0.146984f}, { 0.135299f,  0.489961f, 0.000000f}, {-0.109656f, -0.045060f, -0.098212f}, { 0.301777f, -0.140165f,  0.125000f}, {-0.175656f, -0.192044f, -0.083260f}},
		{{  1.473863f,    0.146984f,  -0.636379f}, { 1.374552f, -0.115485f,  1.111096f}, {-0.591956f, -0.079547f, -0.067743f}, {-0.366410f,  0.203873f,  0.649616f}, { 0.101564f, -0.146984f, 0.293969f}, { 0.085231f, -0.040553f, -0.625401f}, { 0.885925f,  0.192044f, -0.290130f}, { 0.053225f, -0.172835f,  0.170603f}},
		{{  0.250000f,    0.125000f,  -0.250000f}, { 1.888366f,  1.576093f,  0.320364f}, {-0.230970f, -0.067650f, -0.230970f}, { 0.322029f, -0.070152f,  0.112497f}, { 0.000000f, -0.125000f, 0.000000f}, {-0.147771f, -0.197210f, -0.075168f}, { 0.095671f,  0.163320f,  0.095671f}, { 0.428410f,  0.186056f, -0.063724f}},
		{{  0.471524f,    0.098212f,   0.425215f}, { 0.977105f, -0.077165f, -0.347380f}, {-0.117747f, -0.053152f, -0.830959f}, { 0.512008f,  0.136224f,  0.051376f}, {-0.020202f, -0.098212f, 0.196424f}, {-0.310367f, -0.027097f, -0.162179f}, {-0.078676f,  0.128320f,  0.131587f}, {-0.259331f, -0.115485f,  0.330766f}},
		{{  0.013598f,   -0.744145f,  -0.135299f}, {-0.222153f, -0.053152f,  0.173380f}, { 0.051777f, -0.390165f, -0.125000f}, {-0.361946f,  0.093833f,  0.060883f}, {-0.326641f,  0.202949f, 0.000000f}, { 0.117302f, -0.018664f, -0.040681f}, {-0.082107f, -0.058058f,  0.051777f}, {-0.210709f, -0.079547f, -0.034487f}},
		{{  0.120823f,    0.034487f,  -0.360480f}, { 0.222380f, -0.027097f,  0.531298f}, {-0.017127f, -0.018664f,  0.179196f}, { 0.106570f,  0.047835f,  0.247443f}, {-0.501836f, -0.034487f, 0.068975f}, {-0.206507f, -0.009515f, -0.210231f}, {-0.086101f,  0.045060f, -0.148883f}, { 0.232679f, -0.040553f, -0.013796f}}
	};

	// Prepare data
	Image img(8, 8);
	
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			img.SetPixel(i, j, Pixel{ pixels[i][j] });
		}
	}
	FloatImage image(img);

	// Apply DCT
	DCT dct(image);
	auto result = dct.dct();

	// Check
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			auto data = result->Get(i, j);
			EXPECT_NEAR(data.data[0], reference[i][j][0], 0.445f);
			EXPECT_NEAR(data.data[1], reference[i][j][1], 0.445f);
			EXPECT_NEAR(data.data[2], reference[i][j][2], 0.445f);
		}
	}
}

// NOTE: Input and output values for test were generated from MATLAB
TEST(DCTTest, iDCTOneBlock)
{
	float input[8][8][3] = {
		{{1136.f, 1275.f, 799.f}, { 55.f, -18.f, 18.f}, {-10.f, 0.f, 0.f}, {  0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}},
		{{ -12.f,    0.f,   0.f}, {-48.f,   0.f,  0.f}, { 14.f, 0.f, 0.f}, {-19.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}},
		{{  28.f,    0.f,   0.f}, {-13.f,   0.f,  0.f}, {  0.f, 0.f, 0.f}, {  0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}},
		{{   0.f,    0.f,   0.f}, { 17.f,   0.f,  0.f}, {  0.f, 0.f, 0.f}, {  0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}},
		{{   0.f,    0.f,   0.f}, {  0.f,   0.f,  0.f}, {  0.f, 0.f, 0.f}, {  0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}},
		{{   0.f,    0.f,   0.f}, {  0.f,   0.f,  0.f}, {  0.f, 0.f, 0.f}, {  0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}},
		{{   0.f,    0.f,   0.f}, {  0.f,   0.f,  0.f}, {  0.f, 0.f, 0.f}, {  0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}},
		{{   0.f,    0.f,   0.f}, {  0.f,   0.f,  0.f}, {  0.f, 0.f, 0.f}, {  0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}}
	};

	float reference[8][8][3] = {
		{{140.67061393f, 156.25416035f, 102.99583965f}, {144.77829608f, 156.72927989f, 102.52072011f}, {147.58245932f, 157.60718634f, 101.64281366f}, {145.24674741f, 158.75422640f, 100.49577360f}, {140.66158910f, 159.99577360f, 99.25422640f}, {140.12804020f, 161.14281366f, 98.10718634f}, {145.48084683f, 162.02072011f, 97.22927989}, {151.39069200, 162.49583965, 96.75416035f}},
		{{137.61887360f, 156.25416035f, 102.99583965f}, {141.40257524f, 156.72927989f, 102.52072011f}, {144.27355590f, 157.60718634f, 101.64281366f}, {142.81504258f, 158.75422640f, 100.49577360f}, {139.33486643f, 159.99577360f, 99.25422640f}, {139.11285272f, 161.14281366f, 98.10718634f}, {143.73250792f, 162.02072011f, 97.22927989}, {148.75270918, 162.49583965, 96.75416035f}},
		{{137.02507055f, 156.25416035f, 102.99583965f}, {139.61887571f, 156.72927989f, 102.52072011f}, {141.52081544f, 157.60718634f, 101.64281366f}, {140.25572927f, 158.75422640f, 100.49577360f}, {137.27226079f, 159.99577360f, 99.25422640f}, {136.19846457f, 161.14281366f, 98.10718634f}, {138.37092913f, 162.02072011f, 97.22927989}, {141.15602423, 162.49583965, 96.75416035f}},
		{{143.10921958f, 156.25416035f, 102.99583965f}, {143.44971233f, 156.72927989f, 102.52072011f}, {142.79462493f, 157.60718634f, 101.64281366f}, {140.09560289f, 158.75422640f, 100.49577360f}, {135.93548594f, 159.99577360f, 99.25422640f}, {132.06212159f, 161.14281366f, 98.10718634f}, {129.74626336f, 162.02072011f, 97.22927989}, {128.91241381, 162.49583965, 96.75416035f}},
		{{153.44005853f, 156.25416035f, 102.99583965f}, {151.21280161f, 156.72927989f, 102.52072011f}, {147.55205003f, 157.60718634f, 101.64281366f}, {142.99003995f, 158.75422640f, 100.49577360f}, {137.21980454f, 159.99577360f, 99.25422640f}, {130.00530244f, 161.14281366f, 98.10718634f}, {122.59336068f, 162.02072011f, 97.22927989}, {117.71361175, 162.49583965, 96.75416035f}},
		{{161.43159401f, 156.25416035f, 102.99583965f}, {157.47628357f, 156.72927989f, 102.52072011f}, {152.22905669f, 157.60718634f, 101.64281366f}, {147.50119783f, 158.75422640f, 100.49577360f}, {141.92688153f, 159.99577360f, 99.25422640f}, {133.18089840f, 161.14281366f, 98.10718634f}, {122.25118569f, 162.02072011f, 97.22927989}, {114.27775098, 162.49583965, 96.75416035f}},
		{{163.53524276f, 156.25416035f, 102.99583965f}, {159.13286405f, 156.72927989f, 102.52072011f}, {154.28916382f, 157.60718634f, 101.64281366f}, {151.54807808f, 158.75422640f, 100.49577360f}, {148.41157314f, 159.99577360f, 99.25422640f}, {140.60715345f, 161.14281366f, 98.10718634f}, {128.60281769f, 162.02072011f, 97.22927989}, {119.13710504, 162.49583965, 96.75416035f}},
		{{162.39088889f, 156.25416035f, 102.99583965f}, {158.18978854f, 156.72927989f, 102.52072011f}, {154.38345757f, 157.60718634f, 101.64281366f}, {153.78765751f, 158.75422640f, 100.49577360f}, {153.12870232f, 159.99577360f, 99.25422640f}, {146.90390491f, 161.14281366f, 98.10718634f}, {135.13696955f, 162.02072011f, 97.22927989}, {125.30687187, 162.49583965, 96.75416035f}}
	};
	
	FloatImage image(8, 8);

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			image.Set(i, j, input[i][j]);
		}
	}

	// Apply DCT
	DCT idct(image);
	auto result = idct.idct();

	// Check
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			auto data = result->Get(i, j);
			EXPECT_NEAR(data.data[0], reference[i][j][0], 0.445f);
			EXPECT_NEAR(data.data[1], reference[i][j][1], 0.445f);
			EXPECT_NEAR(data.data[2], reference[i][j][2], 0.445f);
		}
	}
}