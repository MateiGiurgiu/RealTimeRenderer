#pragma once
#include "Mesh.h"
#include "Shader.h"


class MeshRenderer
{
public:
	MeshRenderer(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Shader>& shader, ID3D11Device1* device);
	~MeshRenderer() = default;

	void Draw(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
	void Draw(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, ID3D11RasterizerState* rState);

	// draws the mesh with a temporary replacement shader, the m_shader remains intact
	void DrawWithShader(ID3D11DeviceContext1* context, ID3D11Device1* device, std::shared_ptr<Shader>& shader, DirectX::SimpleMath::Matrix world, 
		DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

private:
	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Shader> m_shader;

	ID3DX11EffectMatrixVariable* m_pWorldVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pViewVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pProjVariable = nullptr;
};

