#include "pch.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Shader.h"

std::map<wchar_t*, std::shared_ptr<Mesh>> ResourceManager::m_meshes = {};
std::map<wchar_t*, std::shared_ptr<Shader>> ResourceManager::m_shaders = {};

std::shared_ptr<Mesh> ResourceManager::GetMesh(wchar_t* filename, ID3D11Device1* device)
{
	// check if this mesh was already loaded
	if(m_meshes.count(filename) == 1)
	{
		return m_meshes[filename];
	}
	else
	{
		m_meshes[filename] = std::make_shared<Mesh>(filename, device);
		return m_meshes[filename];
	}
}

std::shared_ptr<Shader> ResourceManager::GetShader(wchar_t* filename, ID3D11Device1* device)
{
	// check if this shader was already loaded
	if (m_meshes.count(filename) == 1)
	{
		return m_shaders[filename];
	}
	else
	{
		m_shaders[filename] = std::make_shared<Shader>(filename, device);
		return m_shaders[filename];
	}
}
