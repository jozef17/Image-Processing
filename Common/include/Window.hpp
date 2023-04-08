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

	void SetImage(std::shared_ptr<Image> image);
	void SetImage(std::unique_ptr<Image> image);
	void SetTitle(std::string title);

	void Show();

private:
	Image* GetImage();
	
	void UpdateFramebuffer();

	// Window Initialization
	void Init();
	void CreateWindow();
	void SetCallbacks();

	void HandleJoystick();

	// Callbacks
	void HandleKeyPressed(uint16_t key, uint16_t action);
	void HandleResize(uint32_t width, uint32_t height);

	// Check window view parameters
	void UpdateView();

	// Display data
	Framebuffer framebuffer;
	std::shared_ptr<Image> shptrImage = nullptr;
	std::unique_ptr<Image> uqptrImage = nullptr;

	// Window view 
	void* window; // glfw window
	uint32_t width;
	uint32_t height;
	std::string title;

	// Image location
	int32_t locX;
	int32_t locY;
	uint32_t zoom;

};

#endif /* WINDOW_HPP__ */