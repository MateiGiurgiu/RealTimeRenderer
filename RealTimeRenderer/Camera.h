#pragma once

class Camera
{
public:
	Camera();
	Camera(DirectX::SimpleMath::Vector3 initialPos);
	~Camera();

	void Update(float deltaTime);

	DirectX::SimpleMath::Matrix GetViewMatrix();
	DirectX::SimpleMath::Matrix GetProjectionMatrix();

	float MovementSpeed;

private:
	DirectX::SimpleMath::Vector3 m_pos;
	float m_yaw;
	float m_pitch;

	DirectX::SimpleMath::Vector3 m_lookAt;
};

