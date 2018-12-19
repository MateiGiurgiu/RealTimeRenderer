#pragma once
class Engine
{
public:
	Engine();
	~Engine();

	void Initialize(HWND hwnd, int width, int height);

private:
	HWND m_hwnd;
	int m_width;
	int m_height;
};

