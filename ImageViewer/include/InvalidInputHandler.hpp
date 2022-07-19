#pragma once

#ifndef INVALID_ARGUMENT_HANDLER_HPP__
#define INVALID_ARGUMENT_HANDLER_HPP__

#include "ArgumentHandler.hpp"

class InvalidInputHandler final: public ArgumentHandler
{
public:
	InvalidInputHandler() =  default;
	~InvalidInputHandler() = default;

	virtual int Run() override;

};


#endif /* INVALID_ARGUMENT_HANDLER_HPP__ */
