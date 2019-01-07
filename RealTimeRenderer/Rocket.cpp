#include "pch.h"
#include "Rocket.h"
#include "ResourceManager.h"

using namespace DirectX::SimpleMath;

Rocket::Rocket(ID3D11Device1* device)
{
	std::shared_ptr<Mesh> mesh = ResourceManager::GetMesh(L"Models/Rocket.sdkmesh", device);
	std::shared_ptr<Shader> shader = ResourceManager::GetShader(L"Shaders/Uber.fx", device);
	m_meshRenderer = std::make_shared<MeshRenderer>(mesh, shader, device);

	SetDiffuseTexture(ResourceManager::GetTexture(L"Textures/Rocket_D.png", device));
	SetNormalTexture(ResourceManager::GetTexture(L"Textures/Rocket_N.png", device));
	SetSpecularTexture(ResourceManager::GetTexture(L"Textures/Rocket_S.png", device));

	m_shadowShader = ResourceManager::GetShader(L"Shaders/Shadow.fx", device);
	m_meshRenderer->SetShadowShader(device, m_shadowShader);
}

Rocket::~Rocket()
{
}

void Rocket::RenderDeferred(ID3D11DeviceContext1* context, const Matrix view, const Matrix proj)
{
	if (m_diffuse)
	{
		m_meshRenderer->GetShader()->SetTexture("diffuseTex", *m_diffuse);
	}

	if (m_normal)
	{
		m_meshRenderer->GetShader()->SetTexture("normalTex", *m_normal);
	}

	if (m_specular)
	{
		m_meshRenderer->GetShader()->SetTexture("specularTex", *m_specular);
	}

	m_meshRenderer->Draw(context, GetWorldMatrix(), view, proj);

	m_meshRenderer->GetShader()->SetTexture("specularTex", nullptr);
	m_meshRenderer->GetShader()->SetTexture("normalTex", nullptr);
	m_meshRenderer->GetShader()->SetTexture("diffuseTex", nullptr);
}

void Rocket::RenderShadow(ID3D11DeviceContext1* context, Matrix view, Matrix proj)
{
	if (m_shadowShader)
	{
		m_meshRenderer->DrawShadow(context, GetWorldMatrix(), view, proj);
	}
}