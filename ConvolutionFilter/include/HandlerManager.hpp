#pragma once

#ifndef HANDLER_MANAGER_HPP__
#define HANDLER_MANAGER_HPP__

#include <memory>
#include <vector>
#include <string>
#include <map>

class ArgumentHandler;

class HandlerManager final
{
public:
	HandlerManager() = delete;
	~HandlerManager() = delete;

	static std::unique_ptr<ArgumentHandler> GetHandler(const std::map<std::string, std::vector<std::string>> &options);

};

#endif /* HANDLER_MANAGER_HPP__ */