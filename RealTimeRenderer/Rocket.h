#pragma once
#include "SceneNode.h"
#include "Texture.h"
#include "Shader.h"
#include "MeshRenderer.h"
#include "ParticleSystem.h"
#include "Geometry.h"
#include "VoxelTerrain.h"

class Rocket : public SceneNode
{
public:
	Rocket(ID3D11Device1* device, std::shared_ptr<ParticleSystem> enginePS, std::shared_ptr<ParticleSystem> explosionPS, std::shared_ptr<Geometry> pole, std::shared_ptr<VoxelTerrain> voxelTerrain);
	~Rocket();

	Rocket(const Rocket& other) = default;
	Rocket& operator=(const Rocket& other) = default;

	// Interface implementation
	void RenderDeferred(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;
	void RenderShadow(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

	void SetDiffuseTexture(std::shared_ptr<Texture> const texture) { m_diffuse = texture; }
	void SetNormalTexture(std::shared_ptr<Texture> const texture) { m_normal = texture; }
	void SetSpecularTexture(std::shared_ptr<Texture> const texture) { m_specular = texture; }

	void Update(float deltaTime, float currentTime) override;
	void Reset();

	DirectX::SimpleMath::Color const &  GetLightColor() const { return m_lightColor; }
	DirectX::SimpleMath::Vector3 const & GetLightPosition() const { return m_lightPosition; }

private:
	const float LAUNCH_PITCH_MIN = 10.0;
	const float LAUNCH_PITCH_MAX = 40.0;

	std::shared_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Shader> m_shadowShader;
	std::shared_ptr<Texture> m_diffuse;
	std::shared_ptr<Texture> m_normal;
	std::shared_ptr<Texture> m_specular;

	std::shared_ptr<ParticleSystem> m_enginePS;
	std::shared_ptr<ParticleSystem> m_explosionPS;

	std::shared_ptr<Geometry> m_pole;

	std::shared_ptr<VoxelTerrain> m_voxelTerrain;

	float m_deactivatedTime = 0;
	float m_launchPitch = LAUNCH_PITCH_MIN;
	bool m_launched = false;

	DirectX::SimpleMath::Vector3 m_velocity;

	DirectX::SimpleMath::Color m_lightColor;
	DirectX::SimpleMath::Vector3 m_lightPosition;
};

