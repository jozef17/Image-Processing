#include "Window.hpp"
#include "Image.hpp"
#include "Exception.hpp"

#include <GLFW/glfw3.h>
#include <Pixel.hpp>
#include <iostream>

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
	UpdateFramebuffer();

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

	// Setting callback
//	glfwSetKeyCallback((GLFWwindow*)this->window, keyCallback);
	glfwMakeContextCurrent((GLFWwindow*)this->window);

	this->run = true;
//	this->thread = std::thread(
//		[&]() -> void
		{
			// Displaying framebuffer
			while (!glfwWindowShouldClose((GLFWwindow*)this->window) && this->run)
			{
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
//	);
//	this->thread.detach();
}

void Window::UpdateFramebuffer()
{
	if (this->image == nullptr)
	{
		return;
	}

	auto index = 0;
	for (int i = 0; i < this->height; i++) // y
	{
		for (int j = 0; j < this->width; j++) // x
		{
			auto pixel = this->image->GetPixel(j + this->locX, i + this->locY);
			auto rgb = pixel.ToRGB();
			this->framebuffer[index++] = rgb.red;
			this->framebuffer[index++] = rgb.green;
			this->framebuffer[index++] = rgb.blue;
//			Pixel_t pixel = image->get(i + offset_y, j + offset_x);
//			((Pixel_t*)framebuffer)[index] = pixel;
//			index--;
		}
	}

	// TODO
}
