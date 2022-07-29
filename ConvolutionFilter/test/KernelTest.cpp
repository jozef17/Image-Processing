#include <gtest/gtest.h>
#include "Kernel.hpp"

TEST(KernelTest, IdentityKernel)
{
	double reference[3][3] = { {0,0,0},{0,1,0},{0,0,0} };

	auto identity = Kernel::GetKernel("identity");

	// Check size
	EXPECT_EQ(3, identity.GetWidth());
	EXPECT_EQ(3, identity.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], identity.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  0.00   0.00   0.00 \n  0.00   1.00   0.00 \n  0.00   0.00   0.00 \n",
		identity.ToString());
}

TEST(KernelTest, BoxBlurKernel)
{
	double reference[3][3] = {
		{1.f / 9.f, 1.f / 9.f, 1.f / 9.f},
		{1.f / 9.f, 1.f / 9.f, 1.f / 9.f},
		{1.f / 9.f, 1.f / 9.f, 1.f / 9.f} };

	auto boxBlur = Kernel::GetKernel("boxBlur");

	// Check size
	EXPECT_EQ(3, boxBlur.GetWidth());
	EXPECT_EQ(3, boxBlur.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], boxBlur.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  0.11   0.11   0.11 \n  0.11   0.11   0.11 \n  0.11   0.11   0.11 \n",
		boxBlur.ToString());
}

TEST(KernelTest, EdgeDetectionHorisontalKernel)
{
	double reference[3][3] = { {1,1,1},{0,0,0},{-1,-1,-1} };

	auto edgeDetection = Kernel::GetKernel("edgeDetectionH");

	// Check size
	EXPECT_EQ(3, edgeDetection.GetWidth());
	EXPECT_EQ(3, edgeDetection.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], edgeDetection.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  1.00   1.00   1.00 \n  0.00   0.00   0.00 \n -1.00  -1.00  -1.00 \n",
		edgeDetection.ToString());
}

TEST(KernelTest, EdgeDetectionVerticalKernel)
{
	double reference[3][3] = { {1,0,-1},{1,0,-1},{1,0,-1} };

	auto edgeDetection = Kernel::GetKernel("edgeDetectionV");

	// Check size
	EXPECT_EQ(3, edgeDetection.GetWidth());
	EXPECT_EQ(3, edgeDetection.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], edgeDetection.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  1.00   0.00  -1.00 \n  1.00   0.00  -1.00 \n  1.00   0.00  -1.00 \n",
		edgeDetection.ToString());
}

TEST(KernelTest, GaussianKernel)
{
	double reference[3][3] = {
		{ 1.f / 16.f, 2.f / 16.f, 1.f / 16.f},
		{ 2.f / 16.f, 4.f / 16.f, 2.f / 16.f},
		{ 1.f / 16.f, 2.f / 16.f, 1.f / 16.f} };

	auto gaussian = Kernel::GetKernel("gaussian");

	// Check size
	EXPECT_EQ(3, gaussian.GetWidth());
	EXPECT_EQ(3, gaussian.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], gaussian.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  0.06   0.12   0.06 \n  0.12   0.25   0.12 \n  0.06   0.12   0.06 \n",
		gaussian.ToString());
}

TEST(KernelTest, Gaussian5by5Kernel)
{
	double reference[5][5] = {
		{ 1.f / 256.f,  4.f / 256.f,  6.f / 256.f,  4.f / 256.f, 1.f / 256.f},
		{ 4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f},
		{ 6.f / 256.f, 24.f / 256.f, 36.f / 256.f, 24.f / 256.f, 6.f / 256.f},
		{ 4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f},
		{ 1.f / 256.f,  4.f / 256.f,  6.f / 256.f,  4.f / 256.f, 1.f / 256.f}};

	auto gaussian = Kernel::GetKernel("gaussian5");

	// Check size
	EXPECT_EQ(5, gaussian.GetWidth());
	EXPECT_EQ(5, gaussian.GetHeight());

	// Check content
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 5; i++)
		{
			EXPECT_NEAR(reference[j][i], gaussian.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  0.00   0.02   0.02   0.02   0.00 \n  0.02   0.06   0.09   0.06   0.02 \n  0.02   0.09   0.14   0.09   0.02 \n  0.02   0.06   0.09   0.06   0.02 \n  0.00   0.02   0.02   0.02   0.00 \n",
		gaussian.ToString());
}

TEST(KernelTest, SobelKernel)
{
	double reference[3][3] = { {1,0,-1},{2,0,-2},{1,0,-1} };

	auto sobel = Kernel::GetKernel("sobel");

	// Check size
	EXPECT_EQ(3, sobel.GetWidth());
	EXPECT_EQ(3, sobel.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], sobel.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  1.00   0.00  -1.00 \n  2.00   0.00  -2.00 \n  1.00   0.00  -1.00 \n",
		sobel.ToString());
}

TEST(KernelTest, SharpenKernel)
{
	double reference[3][3] = { {0,-1,0},{-1,5,-1},{0,-1,0} };

	auto sharpen = Kernel::GetKernel("sharpen");

	// Check size
	EXPECT_EQ(3, sharpen.GetWidth());
	EXPECT_EQ(3, sharpen.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], sharpen.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  0.00  -1.00   0.00 \n -1.00   5.00  -1.00 \n  0.00  -1.00   0.00 \n",
		sharpen.ToString());
}

TEST(KernelTest, LaplacianKernel)
{
	double reference[3][3] = { {0,1,0},{1,-4,1},{0,1,0} };

	auto laplacian = Kernel::GetKernel("laplacian");

	// Check size
	EXPECT_EQ(3, laplacian.GetWidth());
	EXPECT_EQ(3, laplacian.GetHeight());

	// Check content
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			EXPECT_NEAR(reference[j][i], laplacian.Get(i, j), 0.001);
		}
	}

	// Check string conversion
	EXPECT_EQ(
		"  0.00   1.00   0.00 \n  1.00  -4.00   1.00 \n  0.00   1.00   0.00 \n",
		laplacian.ToString());
}
