#pragma once

#include "pch.h"
#include <fstream>

struct ShaderBuffer
{
	BYTE* buffer;
	int size;
};

inline ShaderBuffer LoadShader(std::wstring filename)
{
	// load precompiled shaders from .cso objects
	ShaderBuffer sb = {};
	BYTE* fileData = nullptr;

	// open the file
	std::ifstream csoFile(filename, std::ios::in | std::ios::binary | std::ios::ate);

	if (csoFile.is_open())
	{
		// get shader size
		sb.size = (unsigned int)csoFile.tellg();

		// collect shader data
		fileData = new byte[sb.size];
		csoFile.seekg(0, std::ios::beg);
		csoFile.read(reinterpret_cast<char*>(fileData), sb.size);
		csoFile.close();
		sb.buffer = fileData;
	}
	else
	{
		LOG("Could not open the file");
	}

	// return the shader buffer
	return sb;
}