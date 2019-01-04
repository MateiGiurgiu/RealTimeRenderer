#pragma once
#include "Direct3D.h"

class RenderSystem
{
public:
	RenderSystem();
	~RenderSystem();

private:
	std::unique_ptr<DX::Direct3D> m_Direct3D;
};

