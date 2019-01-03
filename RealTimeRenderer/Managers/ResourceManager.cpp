#include "pch.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Shader.h"

std::map<wchar_t*, std::shared_ptr<Mesh>> ResourceManager::m_meshes = {};
std::map<wchar_t*, std::shared_ptr<Shader>> ResourceManager::m_shaders = {};

std::shared_ptr<Mesh> ResourceManager::GetMesh(const wchar_t* filename, ID3D11Device1* device)
{
	// check if this mesh was already loaded
	if(m_meshes.count(const_cast<wchar_t*>(filename)) == 1)
	{
		return m_meshes[const_cast<wchar_t*>(filename)];
	}
	else
	{
		m_meshes[const_cast<wchar_t*>(filename)] = std::make_shared<Mesh>(filename, device);
		return m_meshes[const_cast<wchar_t*>(filename)];
	}
}

std::shared_ptr<Shader> ResourceManager::GetShader(const wchar_t* filename, ID3D11Device1* device)
{
	// check if this shader was already loaded
	if (m_meshes.count(const_cast<wchar_t*>(filename)) == 1)
	{
		return m_shaders[const_cast<wchar_t*>(filename)];
	}
	else
	{
		m_shaders[const_cast<wchar_t*>(filename)] = std::make_shared<Shader>(filename, device);
		return m_shaders[const_cast<wchar_t*>(filename)];
	}
}
