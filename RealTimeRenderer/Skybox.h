#pragma once
#include "MeshRenderer.h"
#include "SceneNode.h"

class Skybox : SceneNode
{
public:
	Skybox(ID3D11Device1* device);
	~Skybox();

	void SetSkyTexture(std::shared_ptr<Texture> texture) { m_skyTexture = texture; }

	void RenderForward(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;

private:
	std::unique_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Texture> m_skyTexture;;
};

