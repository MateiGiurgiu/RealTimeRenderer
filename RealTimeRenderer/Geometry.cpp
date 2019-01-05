#include "pch.h"
#include "Geometry.h"
#include "ResourceManager.h"

using namespace DirectX::SimpleMath;

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

void Geometry::Render(ID3D11DeviceContext1* context, const Matrix view, const Matrix proj)
{
	if (m_diffuse)
	{
		m_meshRenderer->GetShader()->SetTexture("diffuseTex", *m_diffuse);
	}
	else
	{
		m_meshRenderer->GetShader()->SetTexture("diffuseTex", nullptr);
	}

	if (m_normal)
	{
		m_meshRenderer->GetShader()->SetTexture("normalTex", *m_normal);
	}
	else
	{
		m_meshRenderer->GetShader()->SetTexture("normalTex", nullptr);
	}

	m_meshRenderer->Draw(context, GetWorldMatrix(), view, proj);
}
