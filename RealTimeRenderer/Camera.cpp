#include "pch.h"
#include "Camera.h"
#include <Keyboard.h>

using namespace DirectX::SimpleMath;

Camera::Camera()
	: m_pos(Vector3::Zero), m_yaw(0.0f), m_pitch(0.0f)
{
	
}

Camera::Camera(Vector3 initialPos)
	: m_pos(initialPos), m_yaw(0.0f), m_pitch(0.0f)
{
	
}


Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	DirectX::Keyboard::State state = DirectX::Keyboard::Get().GetState();

	// handle camera rotation
	/*
	if (state.W || state.Up)
		m_pitch += 1.0f;
	if (state.S || state.Down)
		m_pitch -= 1.0f;
	if (state.A || state.Left)
		m_yaw -= 1.0f;
	if (state.S || state.Right)
		m_yaw += 1.0f;
	*/

	// handle camera movement
	Vector3 movement = Vector3::Zero;

	if (state.LeftControl && (state.W || state.Up))
		movement.z += 1.0f;
	if (state.LeftControl && (state.S || state.Down))
		movement.z -= 1.0f;
	if (state.LeftControl && (state.A || state.Left))
		movement.x += 1.0f;
	if (state.LeftControl && (state.D || state.Right))
		movement.x -= 1.0f;

	if (state.LeftControl && state.E)
		movement.y += 1.0f;

	if (state.LeftControl && state.Q)
		movement.y -= 1.0f;

	Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.0f);
	movement = Vector3::Transform(movement, q);
	movement *= deltaTime;
	
	m_pos += movement;
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
