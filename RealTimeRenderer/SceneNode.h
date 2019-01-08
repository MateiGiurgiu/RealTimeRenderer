#pragma once
class SceneNode
{
public:
	SceneNode();
	virtual ~SceneNode();

	SceneNode(const SceneNode& other) = default;
	SceneNode& operator=(const SceneNode& other) = default;

	virtual void Update(float const deltaTime, float const currentTime);
	virtual void RenderDeferred(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
	virtual void RenderForward(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
	virtual void RenderShadow(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);

	// Setters
	void SetPosition(const float x, const float y, const float z) { m_position = DirectX::SimpleMath::Vector3(x, y, z); }
	void SetPosition(const DirectX::SimpleMath::Vector3& const pos) { m_position = pos; }
	void SetOrientation(const DirectX::SimpleMath::Vector3& const orient)
	{
		SetOrientation(orient.x, orient.y, orient.z);
	}
	void SetOrientation(const float x, const float y, const float z)
	{
		m_orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(y * DEG2RAD, x * DEG2RAD, z * DEG2RAD);
		m_eulerAngles = DirectX::SimpleMath::Vector3(x, y, z);
	}
	void SetOrientationRadians(const float x, const float y, const float z)
	{
		m_orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(y, x, z);
		m_eulerAngles = DirectX::SimpleMath::Vector3(x * RAD2DEG, y * RAD2DEG, z * RAD2DEG);
	}
	void SetScale(const DirectX::SimpleMath::Vector3 const& scale) { m_scale = scale; }
	void SetScale(float const x, float const y, float const z) { m_scale = DirectX::SimpleMath::Vector3(x, y, z); }

	// Getters
	DirectX::SimpleMath::Vector3 const& GetPosition() const { return m_position; };
	DirectX::SimpleMath::Vector3 const& GetEulerAngles() const { return m_eulerAngles; };
	DirectX::SimpleMath::Quaternion const& GetOrientation() const { return m_orientation; };
	DirectX::SimpleMath::Vector3 const& GetScale() const { return m_scale; };

	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

private:
	const float DEG2RAD = 0.01745329f;
	const float RAD2DEG = 57.2957795f;
	// Transform components
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_eulerAngles;
	DirectX::SimpleMath::Quaternion m_orientation;
	DirectX::SimpleMath::Vector3 m_scale;
};

