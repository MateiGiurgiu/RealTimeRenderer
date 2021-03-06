#pragma once
#include "Texture.h"
#include <d3dx11effect.h>

class Shader
{
public:
	Shader(const wchar_t* filename, ID3D11Device1* device);
	~Shader();

	Shader(const Shader& other) = default;
	Shader& operator=(const Shader& other) = default;

	HRESULT SetInputLayout(const D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT inputDescCount, ID3D11Device1* device);
	void PrepareForDraw(ID3D11DeviceContext1* context, const int passIndex = 0);
	bool IsValid() const { return (m_effect != nullptr && m_effect->IsValid()); }
	ID3DX11EffectPass* GetPass(int passIndex) const;

	// Getters
	ID3DX11Effect* GetEffect() const { return m_effect; };

	// Utilities
	void SetTexture(LPCSTR varName, const Texture& texture);
	void SetTexture(LPCSTR varName, ID3D11ShaderResourceView* texture);
	void SetVector(LPCSTR varName, DirectX::SimpleMath::Vector2 const & vector);
	void SetVector(LPCSTR varName, DirectX::SimpleMath::Vector3 const & vector);
	void SetVector(LPCSTR varName, DirectX::SimpleMath::Vector4 const & vector);
	void SetColor(LPCSTR varName, DirectX::SimpleMath::Color const & color);
	void SetMatrix(LPCSTR varName, DirectX::SimpleMath::Matrix const & matrix);

private:
	ID3D11InputLayout* m_vertexInputLayout = nullptr;
	ID3DX11Effect* m_effect = nullptr;
	ID3DX11EffectTechnique* m_technique = nullptr;
};

