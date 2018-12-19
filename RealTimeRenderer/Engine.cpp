#include "pch.h"
#include "Engine.h"


Engine::Engine()
{
	Log::Print("Engine Created!");
}


Engine::~Engine()
{
}

void Engine::Initialize(HWND hwnd)
{
	m_hwnd = hwnd;
}
