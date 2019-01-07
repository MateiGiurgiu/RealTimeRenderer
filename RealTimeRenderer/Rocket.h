#pragma once
#include "SceneNode.h"
#include "Texture.h"
#include "Shader.h"
#include "MeshRenderer.h"
#include "ParticleSystem.h"
#include "Geometry.h"

class Rocket : public SceneNode
{
public:
	Rocket(ID3D11Device1* device, std::shared_ptr<ParticleSystem> enginePS, std::shared_ptr<ParticleSystem> explosionPS, std::shared_ptr<Geometry> pole);
	~Rocket();

	// Interface implementation
	void RenderDeferred(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;
	void RenderShadow(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;

	void SetDiffuseTexture(std::shared_ptr<Texture> texture) { m_diffuse = texture; }
	void SetNormalTexture(std::shared_ptr<Texture> texture) { m_normal = texture; }
	void SetSpecularTexture(std::shared_ptr<Texture> texture) { m_specular = texture; }

	void Update(float deltaTime, float currentTime) override;
private:
	std::shared_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Shader> m_shadowShader;
	std::shared_ptr<Texture> m_diffuse;
	std::shared_ptr<Texture> m_normal;
	std::shared_ptr<Texture> m_specular;

	std::shared_ptr<ParticleSystem> m_enginePS;
	std::shared_ptr<ParticleSystem> m_explosionPS;

	std::shared_ptr<Geometry> m_pole;

	bool m_launched;
	DirectX::SimpleMath::Vector3 m_velocity;
};

