#include "pch.h"
#include "SceneNode.h"

using namespace DirectX::SimpleMath;

SceneNode::SceneNode()
	: m_position(Vector3::Zero), m_eulerAngles(Vector3::Zero), m_orientation(Quaternion::Identity), m_scale(Vector3::One)
{
}

Matrix SceneNode::GetWorldMatrix() const
{
	const Matrix T = Matrix::CreateTranslation(m_position);
	const Matrix R = Matrix::CreateFromQuaternion(m_orientation);
	const Matrix S = Matrix::CreateScale(m_scale);

	return S * R * T;
}
