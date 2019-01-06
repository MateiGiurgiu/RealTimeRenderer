#pragma once
#include "Mesh.h"
#include "Shader.h"


class MeshRenderer
{
public:
	MeshRenderer(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Shader>& shader, ID3D11Device1* device);
	~MeshRenderer() = default;

	// Only prepares the mesh and shader for rendering, by setting up vertex, index buffer, input layout and shaders, but does not call DrawIndexed.
	void PrepareForDraw(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) const;

#pragma region Draw Normal

	void Draw(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) const;
	void Draw(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, ID3D11RasterizerState* rState) const;

	void DrawShadow(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) const;
	// draws the mesh with a temporary replacement shader, the m_shader remains intact
	void DrawWithShader(ID3D11DeviceContext1* context, ID3D11Device1* device, std::shared_ptr<Shader>& shader, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) const;

#pragma endregion


#pragma  region Draw Instanced

	void SetInstanceData(ID3D11Device1* device, void* data, UINT dataSize, UINT numOfInstances) const;
	void SetInstanceData(ID3D11Device1* device, LPCSTR instanceVarName, void* data, UINT dataSize, UINT numOfInstances) const;
	void DrawInstanced(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj, UINT numOfInstances) const;

#pragma endregion

	std::shared_ptr<Shader> GetShader() const { return m_shader; }
	void SetShadowShader(ID3D11Device1* device, std::shared_ptr<Shader> shadowShader);

private:
	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Shader> m_shadowShader;

	ID3DX11EffectMatrixVariable* m_pWorldVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pViewVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pProjVariable = nullptr;
};

