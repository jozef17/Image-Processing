#pragma once

#ifndef HANDLER_MANAGER_HPP__
#define HANDLER_MANAGER_HPP__

#include <memory>
#include <vector>

class ArgumentHandler;
class CommandLineOption;

class HandlerManager final
{
public:
	HandlerManager() = delete;
	~HandlerManager() = delete;

	static std::unique_ptr<ArgumentHandler> GetHandler(const std::vector<CommandLineOption> &options);

};

#endif /* HANDLER_MANAGER_HPP__ */