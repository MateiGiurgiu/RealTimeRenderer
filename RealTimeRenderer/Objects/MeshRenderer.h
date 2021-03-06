#pragma once
#include "Mesh.h"
#include "Shader.h"


class MeshRenderer
{
public:
	MeshRenderer(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Shader>& shader, ID3D11Device1* const device);
	~MeshRenderer() = default;

	MeshRenderer(const MeshRenderer& other) = default;
	MeshRenderer& operator=(const MeshRenderer& other) = default;

	// Only prepares the mesh and shader for rendering, by setting up vertex, index buffer, input layout and shaders, but does not call DrawIndexed.
	void PrepareForDraw(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) const;

#pragma region Draw Normal

	void Draw(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) const;
	void Draw(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj, ID3D11RasterizerState* rState) const;

	void DrawShadow(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) const;

#pragma endregion


#pragma  region Draw Instanced

	void SetInstanceData(void* data, UINT dataSize, UINT numOfInstances) const;
	void SetInstanceData(LPCSTR instanceVarName, void* data, UINT dataSize, UINT numOfInstances) const;
	void DrawInstanced(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj, UINT numOfInstances) const;
	void DrawShadowInstanced(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj, UINT numOfInstances) const;

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

