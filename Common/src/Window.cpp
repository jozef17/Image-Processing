#include "Window.hpp"
#include "Image.hpp"
#include "Exception.hpp"
#include "Pixel.hpp"

#include <GLFW/glfw3.h>
#include <functional>
#include <map>

// GLFW Callbacks
std::map<void*, std::function<void(uint16_t, uint16_t)>> windowKeyMapping;
std::map<void*, std::function<void(uint16_t, uint16_t)>> windowSizeMapping;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
{
	auto callback = windowKeyMapping.find(window);
	if (callback != windowKeyMapping.end())
	{
		windowKeyMapping[window](key, action);
	}
}

void ResizeCallback(GLFWwindow* window, int width, int height) noexcept
{
	auto callback = windowSizeMapping.find(window);
	if (callback != windowSizeMapping.end())
	{
		windowSizeMapping[window](width, height);
	}
}

Framebuffer::Framebuffer() noexcept : size(0)
{}

void Framebuffer::Resize(uint32_t newSize)
{
	if (this->size < newSize)
	{
		this->size = newSize;
		this->framebuffer = std::unique_ptr<uint8_t[]>(new uint8_t[newSize]);
	}
}

void* Framebuffer::Get()
{
	return framebuffer.get();
}

Window::Window(uint32_t width, uint32_t height)
	: width(width), height(height), locX(0), locY(0),
	zoom(1), uqptrImage(nullptr), shptrImage(nullptr), window(nullptr)
{
	this->framebuffer.Resize(width * height * 3);
}

Window::~Window()
{
	// Cleanup
	if (!this->window)
	{
		windowKeyMapping.erase(this->window);
		windowSizeMapping.erase(this->window);

		glfwDestroyWindow((GLFWwindow*)this->window);
		glfwTerminate();
	}
}

void Window::SetImage(std::shared_ptr<Image> image)
{
	this->shptrImage = image;
	this->uqptrImage = nullptr;

	this->width = std::min(this->shptrImage->GetWidth(), width);
	this->height = std::min(this->shptrImage->GetHeight(), height);
}

void Window::SetImage(std::unique_ptr<Image> image)
{
	this->uqptrImage = std::move(image);
	this->shptrImage = nullptr;

	this->width = std::min(this->uqptrImage->GetWidth(), width);
	this->height = std::min(this->uqptrImage->GetHeight(), height);
}

void Window::SetTitle(std::string title)
{
	this->title = title;
}

void Window::Show()
{
	Init();

	CreateWindow();
	SetCallbacks();


	// Displaying framebuffer
	glfwMakeContextCurrent((GLFWwindow*)this->window);
	while (!glfwWindowShouldClose((GLFWwindow*)this->window))
	{
		HandleJoystick();
		UpdateFramebuffer();

		int w, h;
		glViewport(0, 0, this->width, this->height);
		glfwGetFramebufferSize((GLFWwindow*)this->window, &w, &h);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawPixels(this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->framebuffer.Get());
		glfwSwapBuffers((GLFWwindow*)this->window);
		glfwPollEvents();
	}

	// Cleanup
	glfwDestroyWindow((GLFWwindow*)this->window);
	glfwTerminate();
	this->window = nullptr;
}

Image* Window::GetImage()
{
	return this->shptrImage == nullptr ? this->uqptrImage.get() : this->shptrImage.get();
}

void Window::UpdateFramebuffer()
{
	if (this->shptrImage == nullptr && this->uqptrImage == nullptr)
	{
		return;
	}

	auto image = GetImage();

	auto index = 0;
	uint8_t* framebuffer = (uint8_t*)this->framebuffer.Get();
	for (uint32_t i = 0; i < this->height; i++) // y
	{
		for (uint32_t j = 0; j < this->width; j++) // x
		{
			Pixel pixel;
			if (image->GetStartPosition() == Image::StartPosition::TopLeft)
			{
				pixel = image->GetPixel(
					(j / this->zoom) + this->locX,
					(i / this->zoom) + this->locY
				);
			}
			else
			{
				pixel = image->GetPixel(
					(j / this->zoom) + this->locX,
					(image->GetHeight() - 1) - (i / this->zoom) - this->locY
				);
			}

			auto rgb = pixel.ToRGB();
			std::memcpy(&framebuffer[index], &rgb, 3);
			index += 3;
		}
	}
}

void Window::Init()
{
	if (!glfwInit())
	{
		throw Exception("Failed to initialize glfw");
	}
}

void Window::CreateWindow()
{
	this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);
	if (this->window == nullptr)
	{
		glfwTerminate();
		throw Exception("Failed to create window");
	}
}

void Window::SetCallbacks()
{
	// Register event mapping
	windowKeyMapping[this->window] = std::bind(&Window::HandleKeyPressed, this, std::placeholders::_1, std::placeholders::_2);
	windowSizeMapping[this->window] = std::bind(&Window::HandleResize, this, std::placeholders::_1, std::placeholders::_2);

	// Set Callbacks
	glfwSetKeyCallback((GLFWwindow*)this->window, KeyCallback);
	glfwSetWindowSizeCallback((GLFWwindow*)this->window, ResizeCallback);
}

void Window::HandleJoystick()
{
	// Check if jhoystick is present
	int jpysticPressent = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (!jpysticPressent)
	{
		return;
	}

	// Get Axes
	int axesCount = 0;
	const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

	// Left Joystick
	if (axesCount >= 2)
	{
		int lx = (int)(axes[0] * 5);
		int ly = (int)(axes[1] * -5);

		this->locX += lx;
		this->locY += ly;
	}

	// Right Joystick
	if (axesCount >= 4)
	{
		int rx = (int)(axes[2] * 5);
		int ry = (int)(axes[3] * -5);

		this->locX += rx;
		this->locY += ry;
	}

	// Triggers (for zoom)
	if (axesCount >= 6)
	{
		int zoomIn = (int)(axes[5] * 0.5 + 1);
		int zoomOut = (int)(axes[4] * -0.5 -1);
		this->zoom += zoomIn + zoomOut;
	}

	UpdateView();
}

void Window::HandleKeyPressed(uint16_t key, uint16_t action)
{
	switch (key)
	{
		// Left
	case GLFW_KEY_LEFT:
	case GLFW_KEY_A:
		this->locX--;
		break;
		// Right
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
		this->locX++;
		break;
		// Up
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
		this->locY++;
		break;
		// Down
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
		this->locY--;
		break;
		// Zoom out
	case GLFW_KEY_KP_SUBTRACT:
	case GLFW_KEY_MINUS:
		this->zoom = action == GLFW_RELEASE ? this->zoom - 1 : this->zoom;
		break;
		// Zoom in
	case GLFW_KEY_KP_ADD:
	case GLFW_KEY_EQUAL:
		this->zoom = action == GLFW_RELEASE ? this->zoom + 1 : this->zoom;
		break;
	} // switch

	UpdateView();
}

void Window::HandleResize(uint32_t width, uint32_t height)
{
	// Update window dimmensions
	auto image = GetImage();
	this->width = std::min(image->GetWidth(), width);
	this->height = std::min(image->GetHeight(), height);

	// Update framebuffer size
	this->framebuffer.Resize(this->width * this->height * 3);

	UpdateView();
}

void Window::UpdateView()
{
	// zoom 1-5
	this->zoom = std::max((uint32_t)1, this->zoom);
	this->zoom = std::min((uint32_t)5, this->zoom);

	// Left and bottom image borders
	this->locX = std::max(0, this->locX);
	this->locY = std::max(0, this->locY);

	// Left and bottom image borders
	auto image = GetImage();
	int32_t limitX = image->GetWidth() - (this->width / this->zoom);
	int32_t limitY = image->GetHeight() - (this->height / this->zoom);
	this->locX = std::min(this->locX, limitX);
	this->locY = std::min(this->locY, limitY);
}