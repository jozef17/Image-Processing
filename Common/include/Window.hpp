#pragma once

#ifndef WINDOW_HPP__
#define WINDOW_HPP__

#include <cstdint>
#include <memory>
#include <string>

class Image;

class Framebuffer
{
public:
	Framebuffer() noexcept;

	void Resize(uint32_t newSize);
	void* Get();

private:
	std::unique_ptr<uint8_t[]> framebuffer;
	uint32_t size;

};

class Window final
{
public:
	Window(uint32_t width, uint32_t height);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();

	void SetImage(std::unique_ptr<Image> image);
	void SetTitle(std::string title);

	void Show();

private:
	void UpdateFramebuffer();

	// Window Initialization
	void Init();
	void CreateWindow();
	void SetCallbacks();

	// Callbacks
	void HandleKeyPressed(uint16_t key, uint16_t action);
	void HandleResize(uint32_t width, uint32_t height);

	// Check window view parameters
	void UpdateView();

	// Display data
	std::unique_ptr<Image> image;
	Framebuffer framebuffer;

	// Window view 
	std::string title;
	uint32_t width;
	uint32_t height;
	void* window; // glfw window

	// Image location
	int32_t locX;
	int32_t locY;
	uint32_t zoom;

};

#endif /* WINDOW_HPP__ */