//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "pch.h"

using namespace DX;

const char* com_exception::what() const
{
	static char s_str[64] = {};
	sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
	return s_str;
}
