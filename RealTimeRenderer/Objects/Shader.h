#pragma once
#include <d3dx11effect.h>

class Shader
{
public:
	Shader(const wchar_t* filename, ID3D11Device1* device);
	~Shader();

	HRESULT SetInputLayout(D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT inputDescCount, ID3D11Device1* device);
	void PrepareForDraw(ID3D11DeviceContext1* context, const int passIndex = 0);
	bool IsValid() const { return m_effect != nullptr; }
	ID3DX11EffectPass* GetPass(int passIndex) const;

	// Getters
	ID3DX11Effect* GetEffect() const { return m_effect; };

	// Utilities
	void SetTexture(LPCSTR varName, ID3D11ShaderResourceView* texture);

private:
	ID3D11InputLayout* m_vertexInputLayout = nullptr;
	ID3DX11Effect* m_effect = nullptr;
	ID3DX11EffectTechnique* m_technique = nullptr;
};

