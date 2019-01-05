#include "pch.h"
#include "Skybox.h"
#include "ResourceManager.h"


Skybox::Skybox(ID3D11Device1* device)
{
	m_meshRenderer = std::make_unique<MeshRenderer>(
		ResourceManager::GetMesh(L"Models/Skybox.sdkmesh", device),
		ResourceManager::GetShader(L"Shaders/Skybox.fx", device),
		device
		);

	SetScale(200, 200, 200);
}


Skybox::~Skybox()
{
}

void Skybox::Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	if (m_skyTexture)
	{
		m_meshRenderer->GetShader()->SetTexture("skyTex", *m_skyTexture);
	}
	else
	{
		m_meshRenderer->GetShader()->SetTexture("skyTex", nullptr);
	}

	m_meshRenderer->Draw(context, GetWorldMatrix(), view, proj);
}
