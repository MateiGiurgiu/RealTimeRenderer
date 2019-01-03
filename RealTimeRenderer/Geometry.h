#pragma once
#include "MeshRenderer.h"

class Geometry
{
public:
	Geometry();
	Geometry(ID3D11Device1* device, const wchar_t* meshName, const wchar_t* shaderName);
	~Geometry();


private:
	std::shared_ptr<MeshRenderer> m_meshRenderer;
};

