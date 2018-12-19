#pragma once
class Engine
{
public:
	Engine();
	~Engine();

	void Initialize(HWND hwnd);

private:
	HWND m_hwnd;

};

