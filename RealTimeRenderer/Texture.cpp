#include "pch.h"
#include "Texture.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <minwinbase.h>

Texture::Texture(ID3D11Device1* device, const std::wstring& filename)
{
	ZeroMemory(&m_textureDesc, sizeof(m_textureDesc));

	const unsigned int startLocation = filename.find_first_of('.');
	const std::wstring extension = filename.substr(startLocation + 1);

	if (extension == L"dds" || extension == L"DDS")
	{
		DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device, filename.c_str(), m_resource.ReleaseAndGetAddressOf(), m_shaderResourceView.ReleaseAndGetAddressOf()));
		DX::ThrowIfFailed(m_resource.As(&m_texture2D));

		m_texture2D->GetDesc(&m_textureDesc);
	}
	else if (extension == L"jpg" || extension == L"JPG")
	{	
		DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, filename.c_str(), m_resource.ReleaseAndGetAddressOf() , m_shaderResourceView.ReleaseAndGetAddressOf()));
		DX::ThrowIfFailed(m_resource.As(&m_texture2D));

		m_texture2D->GetDesc(&m_textureDesc);
	}
}

