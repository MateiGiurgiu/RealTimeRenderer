#include "pch.h"
#include "DirectionalLight.h"
#include <complex>

using namespace DirectX::SimpleMath;

DirectionalLight::DirectionalLight()
	: m_lightColor(Color(1, 1, 1, 1)), m_lightIntensity(1.0f)
{

}

DirectionalLight::DirectionalLight(Vector3 orientation)
	: m_lightColor(Color(1, 1, 1, 1)), m_lightIntensity(1.0f)
{
	SetOrientation(orientation);
}

DirectionalLight::DirectionalLight(float x, float y, float z)
	: m_lightColor(Color(1, 1, 1, 1)), m_lightIntensity(1.0f)
{
	SetOrientation(x, y, z);
}


DirectX::SimpleMath::Matrix DirectionalLight::GetViewMatrix() const
{
	Vector3 lookAt = GetPosition() + GetLightDir();
	return Matrix::CreateLookAt(GetPosition(), lookAt, Vector3::UnitY);
}

DirectX::SimpleMath::Matrix DirectionalLight::GetProjectionMatrix() const
{
	return Matrix::CreateOrthographic(20, 20, 0.1, 15);
}

Vector3 DirectionalLight::GetLightDir() const
{
	Vector3 forward = Vector3::Transform(Vector3(0, 0, 1), m_orientation);
	forward.Normalize();
	return forward;
}

Color DirectionalLight::GetLightColor() const
{
	return Color(m_lightColor.x, m_lightColor.y, m_lightColor.z, m_lightIntensity);
}

void DirectionalLight::Update(float deltaTime, float currentTime)
{
	float timeScale = 0.25f;
	float angleY = std::sin(currentTime * timeScale);

	float signY = 1.0f;

	m_lightIntensity = abs(angleY);
	

	if(angleY < 0)
	{
		angleY *= -1;
		signY = -1.0f;
		m_lightIntensity *= 0.2;
	}
	if (m_lightIntensity < 0.2f) m_lightIntensity = 0.2f;

	float angleZ = std::cos(currentTime * timeScale) * signY;

	SetPosition(0, angleY * 8, angleZ * 8);
	SetOrientationRadians(angleZ + DirectX::XM_PI / 2, 0, 0);
}
