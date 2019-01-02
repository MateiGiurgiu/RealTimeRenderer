#pragma once
#include <map>
#include "Mesh.h"
#include "Shader.h"

class ResourceManager
{
public:
	ResourceManager() = delete;
	~ResourceManager() = delete;

	static std::shared_ptr<Mesh> GetMesh(wchar_t* filename,  ID3D11Device1* device);
	static std::shared_ptr<Shader> GetShader(wchar_t* filename, ID3D11Device1* device);

private:
	static std::map<wchar_t*, std::shared_ptr<Mesh>> m_meshes;
	static std::map<wchar_t*, std::shared_ptr<Shader>> m_shaders;
};

