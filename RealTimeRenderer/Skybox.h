#pragma once
#include "MeshRenderer.h"
#include "SceneNode.h"

class Skybox : SceneNode
{
public:
	explicit Skybox(ID3D11Device1* device);
	~Skybox();

	void SetSkyTexture(std::shared_ptr<Texture> const texture) { m_skyTexture = texture; }

	void RenderForward(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

private:
	std::unique_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Texture> m_skyTexture;;
};

