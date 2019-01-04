#pragma once
#include "Shader.h"

class RenderQuad
{
public:
	RenderQuad(ID3D11Device1* device, UINT screenWidth, UINT screenHeight);
	RenderQuad(ID3D11Device1* device, std::shared_ptr<Shader> shader, UINT screenWidth, UINT screenHeight);
	~RenderQuad() = default;

	void PrepareForDraw(ID3D11DeviceContext1* context);
	void Draw(ID3D11DeviceContext1* context);
	void DrawWithShader(ID3D11DeviceContext1* context, ID3D11Device1* device, Shader& shader);

	std::shared_ptr<Shader> GetShader() const { return m_shader; };

private:
	void CreateGeometryData(ID3D11Device1* device);

	DirectX::SimpleMath::Matrix m_projectionMatrix;
	struct Vertex
	{
		DirectX::XMFLOAT4 Pos;
		DirectX::XMFLOAT2 TexCoord;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	std::shared_ptr<Shader> m_shader;

	unsigned int m_layoutDescCount;
	D3D11_INPUT_ELEMENT_DESC m_layoutDesc[2];

	UINT m_offset;
	UINT m_stride;
	UINT m_indexCount;

};

