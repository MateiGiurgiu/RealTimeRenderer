#pragma once

class Camera
{
public:
	Camera() = default;
	Camera(DirectX::SimpleMath::Vector3 initialPos, float screenWidth, float screenHeight, float fov = 45.0f, float nearPlane = 0.1f, float farPlane = 500);
	~Camera();

	void Update(float deltaTime);

	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
	DirectX::SimpleMath::Vector3 GetViewDirection() const;

	float MovementSpeed;
	float RotationSpeed;

private:
	DirectX::SimpleMath::Vector3 m_pos;
	float m_yaw;
	float m_pitch;

	DirectX::SimpleMath::Matrix m_projection;
	DirectX::SimpleMath::Vector3 m_lookAt;
};

