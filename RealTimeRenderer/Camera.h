#pragma once

class Camera
{
public:
	Camera() = default;
	Camera(const DirectX::SimpleMath::Vector3& initialPos, float screenWidth, float screenHeight, float fov = 45.0f, float nearPlane = 0.1f, float farPlane = 500);
	~Camera();

	void Update(float deltaTime);

	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	const DirectX::SimpleMath::Matrix& GetProjectionMatrix() const;
	DirectX::SimpleMath::Vector3 GetViewDirection() const;

	float GetMovementSpeed() const { return m_movementSpeed; }
	float GetRotationSpeed() const { return m_rotationSpeed; }

	void SetMovementSpeed(const float newSpeed) { m_movementSpeed = newSpeed; }
	void SetRotationSpeed(const float newSpeed) { m_rotationSpeed = newSpeed; }

private:
	DirectX::SimpleMath::Vector3 m_pos;
	float m_yaw;
	float m_pitch;

	DirectX::SimpleMath::Matrix m_projection;
	DirectX::SimpleMath::Vector3 m_lookAt;

	float m_movementSpeed;
	float m_rotationSpeed;
};

