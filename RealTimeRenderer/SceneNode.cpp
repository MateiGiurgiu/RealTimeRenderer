#include "pch.h"
#include "SceneNode.h"

using namespace DirectX::SimpleMath;

SceneNode::SceneNode()
	: m_position(Vector3::Zero), m_eulerAngles(Vector3::Zero), m_orientation(Quaternion::Identity), m_scale(Vector3::One)
{
}

SceneNode::~SceneNode()
{

}

Matrix SceneNode::GetWorldMatrix() const
{
	const Matrix T = Matrix::CreateTranslation(m_position);
	const Matrix R = Matrix::CreateFromQuaternion(m_orientation);
	const Matrix S = Matrix::CreateScale(m_scale);

	return S * R * T;
}

void SceneNode::RenderDeferred(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& const view, const DirectX::SimpleMath::Matrix& const proj) 
{
}

void SceneNode::RenderForward(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& const view, const DirectX::SimpleMath::Matrix& const proj)
{
}

void SceneNode::RenderShadow(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& const view, const DirectX::SimpleMath::Matrix& const proj)
{
}

void SceneNode::Update(float const deltaTime, float const currentTime)
{
}