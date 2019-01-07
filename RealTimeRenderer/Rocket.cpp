#include "pch.h"
#include "Rocket.h"
#include "ResourceManager.h"
#include <Keyboard.h>

using namespace DirectX::SimpleMath;

Rocket::Rocket(ID3D11Device1* device, std::shared_ptr<ParticleSystem> enginePS, std::shared_ptr<ParticleSystem> explosionPS, std::shared_ptr<Geometry> pole)
	: m_velocity(Vector3(0.0f, 0.0f, 0.0f)), m_enginePS(enginePS), m_explosionPS(explosionPS), m_pole(pole), m_launched(false), m_launchPitch(LAUNCH_PITCH_MIN)
{
	std::shared_ptr<Mesh> mesh = ResourceManager::GetMesh(L"Models/Rocket.sdkmesh", device);
	std::shared_ptr<Shader> shader = ResourceManager::GetShader(L"Shaders/Uber.fx", device);
	m_meshRenderer = std::make_shared<MeshRenderer>(mesh, shader, device);
	m_shadowShader = ResourceManager::GetShader(L"Shaders/Shadow.fx", device);
	m_meshRenderer->SetShadowShader(device, m_shadowShader);

	SetDiffuseTexture(ResourceManager::GetTexture(L"Textures/Rocket_D.png", device));
	SetNormalTexture(ResourceManager::GetTexture(L"Textures/Rocket_N.png", device));
	SetSpecularTexture(ResourceManager::GetTexture(L"Textures/Rocket_S.png", device));

	SetPosition(5, 0.25f, 0);
	SetScale(0.4f, 0.4f, 0.4f);
	SetOrientation(0, 0, LAUNCH_PITCH_MIN);

	m_pole->SetPosition(5.0f, 0.25f, 0);
	m_pole->SetScale(0.4f, 0.4f, 0.4f);
	m_pole->SetOrientation(0, 0, LAUNCH_PITCH_MIN);

	m_enginePS->SetPosition(5, 0.25f, 0);
	m_enginePS->SetOrientation(0, 0, LAUNCH_PITCH_MIN);
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

void Rocket::Update(float deltaTime, float currentTime)
{
	if (DirectX::Keyboard::Get().GetState().IsKeyDown(DirectX::Keyboard::OemPeriod) && !m_launched)
	{
		m_launchPitch += 1;
		if (m_launchPitch > LAUNCH_PITCH_MAX) m_launchPitch = LAUNCH_PITCH_MAX;

		SetOrientation(0, 0, m_launchPitch);
		m_pole->SetOrientation(0, 0, m_launchPitch);
		m_enginePS->SetOrientation(0, 0, m_launchPitch);
	}

	if (DirectX::Keyboard::Get().GetState().IsKeyDown(DirectX::Keyboard::OemComma) && !m_launched)
	{
		m_launchPitch -= 1;
		if (m_launchPitch < LAUNCH_PITCH_MIN) m_launchPitch = LAUNCH_PITCH_MIN;

		SetOrientation(0, 0, m_launchPitch);
		m_pole->SetOrientation(0, 0, m_launchPitch);
		m_enginePS->SetOrientation(0, 0, m_launchPitch);
	}

	if(DirectX::Keyboard::Get().GetState().F11 && !m_launched)
	{
		m_launched = true;
		Vector3 localUp = Vector3::Transform(Vector3(0, 1, 0), GetOrientation());
		localUp.Normalize();
		m_velocity = localUp * 12;
		m_enginePS->SetEmit(true);
	}

	if (GetPosition().y > 1.1)
	{
		m_velocity.y -= 9.81 * deltaTime;
	}

	if(m_launched)
	{
		float vel = m_velocity.y / 10;
		if (vel > 1.0) vel = 1.0f;
		if (vel < -1.0) vel = -1.0f;
		
		float angle = (vel - 1.0f) * -DirectX::XM_PI / 2;
		if (angle * 57.2957795f > m_launchPitch)
		{
			SetOrientationRadians(0, 0, angle);
		}
	}

	SetPosition(GetPosition() + m_velocity * deltaTime);
	m_enginePS->SetPosition(GetPosition());
}
