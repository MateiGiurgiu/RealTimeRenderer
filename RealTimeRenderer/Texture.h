#pragma once
class Texture
{
public:
	Texture() = default;
	// Constructor for creating non render target texture from a file
	Texture(ID3D11Device1* device, const std::wstring& filename);
	virtual ~Texture() = default;

	float GetWidth() const { return  m_textureDesc.Width; };
	float GetHeight() const { return m_textureDesc.Height; };

	D3D11_TEXTURE2D_DESC GetTextureDesc() const { return m_textureDesc; }
	ID3D11Texture2D* GetTexture2D() const { return m_texture2D.Get(); }
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }

protected:
	D3D11_TEXTURE2D_DESC m_textureDesc;

	Microsoft::WRL::ComPtr<ID3D11Resource> m_resource;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture2D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

};

