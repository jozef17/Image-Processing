#pragma once

#ifndef FILTER_HANDLER_HPP__
#define FILTER_HANDLER_HPP__

#include "ArgumentHandler.hpp"
#include "Filter.hpp"

class FilterHandler final : public ArgumentHandler
{
public:
	FilterHandler(Filter filter) noexcept;
	~FilterHandler();

	virtual int Run() override;

private:
	Filter filter;

};

#endif /* FILTER_HANDLER_HPP__ */