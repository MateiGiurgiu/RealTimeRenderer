#pragma once
#include "MeshRenderer.h"
#include "IRenderable.h"
#include "SceneNode.h"

class Geometry : public SceneNode
{
public:
	Geometry();
	Geometry(ID3D11Device1* device, const wchar_t* meshName, const wchar_t* shaderName);
	~Geometry() override;

	// Interface implementation
	void Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;

	void SetDiffuseTexture(std::shared_ptr<Texture> texture) { m_diffuse = texture; }
	void SetNormalTexture(std::shared_ptr<Texture> texture) { m_normal = texture; }

private:
	std::shared_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Texture> m_diffuse;
	std::shared_ptr<Texture> m_normal;
};

