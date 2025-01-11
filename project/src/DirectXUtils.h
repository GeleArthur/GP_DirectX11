#include "pch.h"
#include <source_location>

inline void CallDirectX(HRESULT result, const std::source_location& location = std::source_location::current()) 
{
	if (FAILED(result)) {
		throw std::exception(std::to_string(result).c_str());
	}
}
