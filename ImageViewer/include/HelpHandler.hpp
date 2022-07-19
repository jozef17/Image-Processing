#pragma once

#ifndef HELP_HANDLER_HPP__
#define HELP_HANDLER_HPP__

#include "ArgumentHandler.hpp"

class HelpHandler final: public ArgumentHandler
{
public:
	HelpHandler() = default;
	~HelpHandler() = default;

	virtual int Run() override;

};


#endif /* HELP_HANDLER_HPP__ */
