#pragma once

#ifndef WINDOW_HPP__
#define WINDOW_HPP__

#include <cstdint>
#include <memory>
#include <string>
#include <thread>

class Image;

class Window final
{
public:
	Window(uint32_t width, uint32_t height);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();

	void SetImage(Image* image);
	void SetTitle(std::string title);

	void Show();

private:
	void UpdateFramebuffer();

	// Display data
	std::unique_ptr<uint8_t[]> framebuffer;
	Image* image;

	// Window properties
	std::string title;
	uint32_t width;
	uint32_t height;
	void* window;

	// Image location
	uint32_t locX;
	uint32_t locY;

	// Multithreading
	std::thread thread;
	bool run;

};

#endif /* WINDOW_HPP__ */