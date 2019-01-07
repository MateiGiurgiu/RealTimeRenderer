#pragma once
#include "SceneNode.h"
class DirectionalLight : public SceneNode
{
public:
	DirectionalLight();
	explicit DirectionalLight(DirectX::SimpleMath::Vector3 orientation);
	DirectionalLight(float x, float y, float z);
	~DirectionalLight() = default;

	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
	DirectX::SimpleMath::Vector3 GetLightDir() const;

	DirectX::SimpleMath::Color GetLightColor() const;
	void SetLightColor(DirectX::SimpleMath::Color color) { m_lightColor = color; }

	void Update(float deltaTime, float currentTime) override;
private:
	DirectX::SimpleMath::Color m_lightColor;
	float m_lightIntensity;
};

