#pragma once

#ifndef FILTER_HPP__
#define FILTER_HPP__

#include "Kernel.hpp"
#include <memory>

class Image;

class Filter final
{
public:
	Filter(std::shared_ptr<Image> image, Kernel kernel) noexcept;
	Filter(const Filter& old);
	Filter& operator= (const class Filter& other);

	~Filter();

	std::shared_ptr<Image> ApplyFilter();

private:
	std::shared_ptr<Image> image;
	Kernel kernel;

};

#endif
