#include "pch.h"
#include "Texture.h"
#include <DDSTextureLoader.h>

Texture::Texture(ID3D11Device1* device, const std::wstring& filename)
	: m_textureDesc(nullptr), m_isRenderTarget(false)
{
	const unsigned int startLocation = filename.find_first_of('.');
	const std::wstring extension = filename.substr(startLocation + 1);

	if (extension == L"dds")
	{
		DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device, filename.c_str(), m_resource.ReleaseAndGetAddressOf(), m_shaderResourceView.ReleaseAndGetAddressOf()));
		DX::ThrowIfFailed(m_resource.As(&m_texture2D));

		m_texture2D->GetDesc(m_textureDesc);
	}
}

Texture::~Texture()
{
	if(m_textureDesc)
	{
		delete m_textureDesc;
		m_textureDesc = nullptr;
	}
}
