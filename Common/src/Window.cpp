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
	zoom(1), image(nullptr), window(nullptr)
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
	this->image = image;
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

void Window::UpdateFramebuffer()
{
	if (this->image == nullptr)
	{
		return;
	}

	auto index = 0;
	uint8_t* framebuffer = (uint8_t*)this->framebuffer.Get();
	for (uint32_t i = 0; i < this->height; i++) // y
	{
		for (uint32_t j = 0; j < this->width; j++) // x
		{
			auto pixel = this->image->GetPixel((j / this->zoom) + this->locX, (i / this->zoom) + this->locY);
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
	this->width = std::min(this->image->GetWidth(), width);
	this->height = std::min(this->image->GetHeight(), height);

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
	int32_t limitX = this->image->GetWidth() - (this->width / this->zoom);
	int32_t limitY = this->image->GetHeight() - (this->height / this->zoom);
	this->locX = std::min(this->locX, limitX);
	this->locY = std::min(this->locY, limitY);
}