#pragma once
class Texture
{
public:
	Texture() = default;
	// Constructor for creating non render target texture from a file
	Texture(ID3D11Device1* device, const std::wstring& filename);
	virtual ~Texture() = default;

	UINT GetWidth() const { return  m_textureDesc.Width; };
	UINT GetHeight() const { return m_textureDesc.Height; };

	D3D11_TEXTURE2D_DESC GetTextureDesc() const { return m_textureDesc; }
	void SetTextureDesc(const D3D11_TEXTURE2D_DESC& textureDesc) { m_textureDesc = textureDesc; }

	ID3D11Texture2D* GetTexture2D() const { return m_texture2D.Get(); }
	void SetTexture2D(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture2D) { m_texture2D = texture2D; }

	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }
	void SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView) { m_shaderResourceView = shaderResourceView; }

	ID3D11Resource* GetResource() const { return m_resource.Get(); }
	void SetResource(const Microsoft::WRL::ComPtr<ID3D11Resource>& resource) { m_resource = resource; }

	

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView() const { return m_shaderResourceView; }
	

private:
	D3D11_TEXTURE2D_DESC m_textureDesc;

	Microsoft::WRL::ComPtr<ID3D11Resource> m_resource;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture2D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

};

