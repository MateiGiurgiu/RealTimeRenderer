#include "pch.h"
#include "Geometry.h"
#include "ResourceManager.h"


Geometry::Geometry()
{
}

Geometry::Geometry(ID3D11Device1* device, const wchar_t* meshName, const wchar_t* shaderName)
{
	std::shared_ptr<Mesh> mesh = ResourceManager::GetMesh(meshName, device);
	std::shared_ptr<Shader> shader = ResourceManager::GetShader(shaderName, device);
	m_meshRenderer = std::make_shared<MeshRenderer>(mesh, shader, device);
}

Geometry::~Geometry()
{
}
