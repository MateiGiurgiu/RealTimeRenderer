#include "pch.h"
#include "Camera.h"
#include <Keyboard.h>

using namespace DirectX::SimpleMath;

Camera::Camera()
	: m_pos(Vector3::Zero), m_yaw(0.0f), m_pitch(0.0f), MovementSpeed(1.0f)
{
	
}

Camera::Camera(Vector3 initialPos)
	: m_pos(initialPos), m_yaw(0.0f), m_pitch(0.0f), MovementSpeed(1.0f)
{
	
}


Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	DirectX::Keyboard::State state = DirectX::Keyboard::Get().GetState();

	// handle camera rotation
	if (!state.LeftControl)
	{
		Vector2 rotation = Vector2::Zero;

		if (state.W || state.Up)
			rotation.y += 1.0f;
		if (state.S || state.Down)
			rotation.y -= 1.0f;
		if (state.A || state.Left)
			rotation.x += 1.0f;
		if (state.D || state.Right)
			rotation.x -= 1.0f;

		// limit pitch to avoid gimbal lock
		m_pitch += rotation.y * deltaTime;
		m_yaw += rotation.x * deltaTime;
		float limit = DirectX::XM_PI / 2.0f - 0.1f;
		m_pitch = std::max(-limit, m_pitch);
		m_pitch = std::min(+limit, m_pitch);

		// keep longitude in sane range by wrapping
		if (m_yaw > DirectX::XM_PI)
		{
			m_yaw -= DirectX::XM_PI * 2.0f;
		}
		else if (m_yaw < -DirectX::XM_PI)
		{
			m_yaw += DirectX::XM_PI * 2.0f;
		}
	}
	else
	{
		// handle camera movement
		Vector3 movement = Vector3::Zero;

		if (state.W || state.Up)
			movement.z += 1.0f;
		if (state.S || state.Down)
			movement.z -= 1.0f;
		if (state.A || state.Left)
			movement.x += 1.0f;
		if (state.D || state.Right)
			movement.x -= 1.0f;

		if (state.E)
			movement.y += 1.0f;

		if (state.Q)
			movement.y -= 1.0f;

		movement.Normalize();

		Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, -m_pitch, 0.0f);
	
		movement = Vector3::Transform(movement, q);
		movement *= deltaTime * MovementSpeed;

		m_pos += movement;
	}
}

Matrix Camera::GetViewMatrix()
{
	float y = sinf(m_pitch);
	float r = cosf(m_pitch);
	float z = r * cosf(m_yaw);
	float x = r * sinf(m_yaw);

	Vector3 lookAt = m_pos + Vector3(x, y, z);

	return Matrix::CreateLookAt(m_pos, lookAt, Vector3::UnitY);
}
