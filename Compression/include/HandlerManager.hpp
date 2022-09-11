#pragma once

#ifndef HANDLER_MANAGER_HPP__
#define HANDLER_MANAGER_HPP__

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "ArgumentHandler.hpp"

class Image;

class HandlerManager final
{
public:
	HandlerManager() = delete;
	~HandlerManager() = delete;

	static std::unique_ptr<ArgumentHandler> GetHandler(const std::map<std::string, std::vector<std::string>> &options);
	
private:
	static float GetQuality(const std::vector<std::string> &arg);
	static std::unique_ptr<Image> GetImage(const std::vector<std::string> &arg);

};

#endif /* HANDLER_MANAGER_HPP__ */