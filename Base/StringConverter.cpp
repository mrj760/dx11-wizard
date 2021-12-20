#include "StringConverter.h"

std::wstring StringConverter::StringToWide(std::string str)
{
	return std::wstring(str.begin(), str.end());
}