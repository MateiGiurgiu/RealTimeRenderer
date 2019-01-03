#pragma once
class Texture
{
public:
	// Constructor for creating non render target texture from a file
	Texture(ID3D11Device1* device, const std::wstring& filename);
	~Texture();

	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }

	float GetWidth() const { return m_textureDesc ? m_textureDesc->Width : 0.0f; };
	float GetHeight() const { return m_textureDesc ? m_textureDesc->Height : 0.0f; };
	D3D11_TEXTURE2D_DESC* GetTextureDesc() const { return m_textureDesc; }

private:
	D3D11_TEXTURE2D_DESC* m_textureDesc;
	
	bool m_isRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture2D;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_resource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
};

