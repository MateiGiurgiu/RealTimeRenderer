#include "pch.h"
#include "DirectionalLight.h"
#include <complex>

using namespace DirectX::SimpleMath;

DirectionalLight::DirectionalLight()
	: m_lightColor(Color(1, 1, 1, 1)), m_lightIntensity(1.0f)
{

}

DirectionalLight::DirectionalLight(const Vector3& orientation)
	: m_lightColor(Color(1, 1, 1, 1)), m_lightIntensity(1.0f)
{
	SetOrientation(orientation);
}

DirectionalLight::DirectionalLight(const float x, const float y, const float z)
	: m_lightColor(Color(1, 1, 1, 1)), m_lightIntensity(1.0f)
{
	SetOrientation(x, y, z);
}

DirectionalLight::~DirectionalLight()
{

}

DirectX::SimpleMath::Matrix DirectionalLight::GetViewMatrix() const
{
	const Vector3 lookAt = GetPosition() + GetLightDir();
	return Matrix::CreateLookAt(GetPosition(), lookAt, Vector3::UnitY);
}

DirectX::SimpleMath::Matrix DirectionalLight::GetProjectionMatrix() const
{
	return Matrix::CreateOrthographic(20, 20, 0.1, 18);
}

Vector3 DirectionalLight::GetLightDir() const
{
	Vector3 forward = Vector3::Transform(Vector3(0, 0, 1), GetOrientation());
	forward.Normalize();
	return forward;
}

Color DirectionalLight::GetLightColor() const
{
	return Color(m_lightColor.x, m_lightColor.y, m_lightColor.z, m_lightIntensity);
}

void DirectionalLight::Update(const float deltaTime, const float currentTime)
{
	const float timeScale = 0.25f;
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

	const float angleZ = std::cos(currentTime * timeScale) * signY;

	SetPosition(0, angleY * 10, angleZ * 10);
	SetOrientationRadians(angleZ + DirectX::XM_PI / 2, 0, 0);
}
