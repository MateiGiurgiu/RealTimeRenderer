#pragma once
#include "Mesh.h"
#include "Shader.h"


class MeshRenderer
{
public:
	MeshRenderer(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Shader>& shader, ID3D11Device1* device);
	~MeshRenderer() = default;

	void Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

private:
	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Shader> m_shader;

	ID3DX11EffectMatrixVariable* m_pWorldVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pViewVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pProjVariable = nullptr;
};

