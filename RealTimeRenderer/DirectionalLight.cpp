#include "pch.h"
#include "DirectionalLight.h"

using namespace DirectX::SimpleMath;

DirectionalLight::DirectionalLight()
{

}

DirectionalLight::DirectionalLight(Vector3 orientation)
{
	SetOrientation(orientation);
}

DirectionalLight::DirectionalLight(float x, float y, float z)
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
