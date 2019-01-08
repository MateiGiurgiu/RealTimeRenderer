#pragma once
#include "MeshRenderer.h"
#include "SceneNode.h"

class Geometry : public SceneNode
{
public:
	Geometry();
	Geometry(ID3D11Device1* device, const wchar_t* meshName, const wchar_t* shaderName);
	~Geometry() override;

	// Interface implementation
	void RenderDeferred(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;
	void RenderShadow(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

	void SetDiffuseTexture(std::shared_ptr<Texture> const texture) { m_diffuse = texture; }
	void SetNormalTexture(std::shared_ptr<Texture> const texture) { m_normal = texture; }
	void SetSpecularTexture(std::shared_ptr<Texture> const texture) { m_specular = texture; }

private:
	std::shared_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Shader> m_shadowShader;
	std::shared_ptr<Texture> m_diffuse;
	std::shared_ptr<Texture> m_normal;
	std::shared_ptr<Texture> m_specular;
};

