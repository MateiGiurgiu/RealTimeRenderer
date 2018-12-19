#include "pch.h"
#include "Engine.h"


Engine::Engine()
{
	LOG("Engine Created!");
}


Engine::~Engine()
{
}

void Engine::Initialize(HWND hwnd, int width, int height)
{
	m_hwnd = hwnd;
	m_width = width;
	m_height = height;

	LOG("Engine Initialized with " << width << " " << height);
}
