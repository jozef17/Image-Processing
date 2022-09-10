#pragma once

#ifndef ARGUMENT_HANDLER_HPP__
#define ARGUMENT_HANDLER_HPP__

class ArgumentHandler
{
public:
	virtual int Run() = 0;
	virtual ~ArgumentHandler() = default;

protected:
	ArgumentHandler() = default;

};

#endif /* ARGUMENT_HANDLER_HPP__ */