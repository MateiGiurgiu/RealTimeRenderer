#pragma once
#include "SceneNode.h"
class DirectionalLight : public SceneNode
{
public:
	DirectionalLight();
	explicit DirectionalLight(const DirectX::SimpleMath::Vector3& orientation);
	DirectionalLight(float x, float y, float z);
	~DirectionalLight() override;

	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
	DirectX::SimpleMath::Vector3 GetLightDir() const;

	DirectX::SimpleMath::Color GetLightColor() const;
	void SetLightColor(const DirectX::SimpleMath::Color& color) { m_lightColor = color; }

	void Update(float deltaTime, float currentTime) override;
private:
	DirectX::SimpleMath::Color m_lightColor;
	float m_lightIntensity;
};

