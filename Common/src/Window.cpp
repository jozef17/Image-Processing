#include "Window.hpp"
#include "Image.hpp"
#include "Exception.hpp"

#include <GLFW/glfw3.h>
#include <Pixel.hpp>
#include <functional>
#include <map>

std::map<void*, std::function<void(uint16_t)>> windowMapping;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto callback = windowMapping.find(window);
	if (callback != windowMapping.end())
	{
		windowMapping[window](key);
	}
}

Window::Window(uint32_t width, uint32_t height) 
	: width(width), height(height), locX(0), locY(0), 
	  image(nullptr), window(nullptr), run(false)
{
	this->framebuffer = std::unique_ptr<uint8_t[]>(new uint8_t[width * height * 3]);
	for (uint32_t i = 0; i < width * height * 3; i++)
	{
		this->framebuffer[i] = 0;
	}
}

Window::~Window()
{
	// Stop thread
	run = false;
	this->thread.join();

	// Cleanup
	if (!this->window)
	{
		windowMapping.erase(this->window);

		glfwDestroyWindow((GLFWwindow*)this->window);
		glfwTerminate();
	}
}

void Window::SetImage(Image* image)
{
	this->image = image;
}

void Window::SetTitle(std::string title)
{
	this->title = title;
}

void Window::Show()
{
	if (this->run)
	{
		throw RuntimeException("Window is already open");
	}

	if (!glfwInit())
	{
		throw Exception("Failed to initialize glfw");
	}

	this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);
	if (this->window == nullptr)
	{
		glfwTerminate();
		throw Exception("Failed to create window");
	}
	windowMapping[this->window] = std::bind(&Window::HandleKeyPress, this, std::placeholders::_1);

	glfwSetKeyCallback((GLFWwindow*)this->window, KeyCallback);
	glfwMakeContextCurrent((GLFWwindow*)this->window);

	this->run = true;
	{
		// Displaying framebuffer
		while (!glfwWindowShouldClose((GLFWwindow*)this->window) && this->run)
		{
			UpdateFramebuffer();

			int w, h;
			glViewport(0, 0, this->width, this->height);
			glfwGetFramebufferSize((GLFWwindow*)this->window, &w, &h);
			glClear(GL_COLOR_BUFFER_BIT);
			glDrawPixels(this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->framebuffer.get());
			glfwSwapBuffers((GLFWwindow*)this->window);
			glfwPollEvents();
		}

		// Cleanup
		glfwDestroyWindow((GLFWwindow*)this->window);
		glfwTerminate();
		this->window = nullptr;
		this->run = false;
	}
}

void Window::UpdateFramebuffer()
{
	if (this->image == nullptr)
	{
		return;
	}

	auto index = 0;
	for (uint32_t i = 0; i < this->height; i++) // y
	{
		for (uint32_t j = 0; j < this->width; j++) // x
		{
			auto pixel = this->image->GetPixel(j + this->locX, i + this->locY);
			auto rgb = pixel.ToRGB();
			this->framebuffer[index++] = rgb.red;
			this->framebuffer[index++] = rgb.green;
			this->framebuffer[index++] = rgb.blue;
		}
	}
}

void Window::HandleKeyPress(uint16_t key)
{
	switch (key)
	{
	case GLFW_KEY_LEFT:
	case GLFW_KEY_A:
		this->locX = this->locX < 1 ? 0 : this->locX - 1;
		break;
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
	{
		auto limitX = this->image->GetWidth() - this->width - 1;
		this->locX = this->locX < limitX ? this->locX + 1 : limitX;
		break;
	}
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
	{
		auto limitY = this->image->GetHeight() - this->height - 1;
		this->locY = this->locY < limitY ? this->locY + 1 : limitY;
		break;
	}
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
		this->locY = this->locY < 1 ? 0 : this->locY - 1;
		break;
	case GLFW_KEY_KP_SUBTRACT:
	case GLFW_KEY_MINUS:
		// TODO zoom out
		break;
	case GLFW_KEY_KP_ADD:
	case GLFW_KEY_EQUAL:
		// TODO zoom in
		break;
	}
}
