#include "Utils.hpp"

bool Utils::EndsWith(const std::string& str, const std::string& ending) noexcept
{
	auto strLen    = str.length();
	auto endingLen = ending.length();
	if (strLen < endingLen)
	{
		return false;
	}

	for (unsigned int i = 0; i < ending.length(); i++)
	{
		if (str.at(strLen - endingLen + i) != ending.at(i))
		{
			return false;
		}
	}

	return true;
}