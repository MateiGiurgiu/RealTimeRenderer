#pragma once
#include <map>
#include "Mesh.h"
#include "Shader.h"

class ResourceManager
{
public:
	ResourceManager() = delete;
	~ResourceManager() = delete;

	static std::shared_ptr<Mesh> GetMesh(const wchar_t* filename,  ID3D11Device1* device);
	static std::shared_ptr<Shader> GetShader(const wchar_t* filename, ID3D11Device1* device);
	static std::shared_ptr<Texture> GetTexture(const std::wstring& filename, ID3D11Device1* device);

	static int MeshesLoaded;
	static int ShadersLoaded;

private:
	static std::map<wchar_t*, std::shared_ptr<Mesh>> m_meshes;
	static std::map<wchar_t*, std::shared_ptr<Shader>> m_shaders;
	static std::map<std::wstring, std::shared_ptr<Texture>> m_textures;
};

