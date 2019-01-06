#pragma once
#include "SceneNode.h"
class DirectionalLight : public SceneNode
{
public:
	DirectionalLight();
	DirectionalLight(DirectX::SimpleMath::Vector3 orientation);
	DirectionalLight(float x, float y, float z);
	~DirectionalLight() = default;

	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
	DirectX::SimpleMath::Vector3 GetLightDir() const;
};

