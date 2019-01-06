#pragma once
class SceneNode
{
public:
	const float DEG2RAD = 0.01745329f;

	SceneNode();
	virtual ~SceneNode() = default;

	virtual void Update(float deltaTime, float currentTime) {};
	virtual void RenderDeferred(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) {}
	virtual void RenderForward(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) {}
	virtual void RenderShadow(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) {}

	// Setters
	void SetPosition(const float x, const float y, const float z) { m_position = DirectX::SimpleMath::Vector3(x, y, z); }
	void SetPosition(const DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
	void SetOrientation(const DirectX::SimpleMath::Vector3 orient)
	{
		SetOrientation(orient.x, orient.y, orient.z);
	}
	void SetOrientation(const float x, const float y, const float z)
	{
		m_orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(y * DEG2RAD, x * DEG2RAD, z * DEG2RAD);
		m_eulerAngles = DirectX::SimpleMath::Vector3(x, y, z);
	}
	void SetScale(const DirectX::SimpleMath::Vector3 scale) { m_scale = scale; }
	void SetScale(float x, float y, float z) { m_scale = DirectX::SimpleMath::Vector3(x, y, z); }

	// Getters
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; };
	DirectX::SimpleMath::Vector3 GetEulerAngles() const { return m_eulerAngles; };
	DirectX::SimpleMath::Quaternion GetOrientation() const { return m_orientation; };
	DirectX::SimpleMath::Vector3 GetScale() const { return m_scale; };

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

protected:
	// Transform components
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_eulerAngles;
	DirectX::SimpleMath::Quaternion m_orientation;
	DirectX::SimpleMath::Vector3 m_scale;
};

