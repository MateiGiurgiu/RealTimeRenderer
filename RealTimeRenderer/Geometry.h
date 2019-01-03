#pragma once
#include "MeshRenderer.h"
#include "IRenderable.h"

class Geometry : public IRenderable
{
public:
	Geometry();
	Geometry(ID3D11Device1* device, const wchar_t* meshName, const wchar_t* shaderName);
	virtual ~Geometry();

	// Interface implementation
	void Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;

	// Setters
	void SetPosition(const float x, const float y, const float z) { m_position = DirectX::SimpleMath::Vector3(x, y, z); }
	void SetPosition(const DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
	void SetOrientation(const DirectX::SimpleMath::Vector3 orient)
	{
		m_orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(orient.y, orient.x, orient.z);
		m_eulerAngles = DirectX::SimpleMath::Vector3(orient.y, orient.x, orient.z);
	}
	void SetScale(const DirectX::SimpleMath::Vector3 scale) { m_scale = scale; }

	// Getters
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; };
	DirectX::SimpleMath::Vector3 GetEulerAngles() const { return m_eulerAngles; };
	DirectX::SimpleMath::Quaternion GetOrientation() const { return m_orientation; };
	DirectX::SimpleMath::Vector3 GetScale() const { return m_scale; };

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

private:
	std::shared_ptr<MeshRenderer> m_meshRenderer;

	// Transform components
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_eulerAngles;
	DirectX::SimpleMath::Quaternion m_orientation;
	DirectX::SimpleMath::Vector3 m_scale;
};

