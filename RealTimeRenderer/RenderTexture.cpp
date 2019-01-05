#include "pch.h"
#include "RenderTexture.h"

RenderTexture::RenderTexture(ID3D11Device1* device, DXGI_FORMAT format, UINT width, UINT height, bool useWithShader)
	: m_useWithShader(useWithShader)
{
	UINT formatSupport = 0;
	if (FAILED(device->CheckFormatSupport(format, &formatSupport)))
	{
		throw std::exception("CheckFormatSupport");
	}

	const UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
	if ((formatSupport & required) != required)
	{
#ifdef _DEBUG
		char buff[128] = {};
		LOG_ERROR("RenderTexture: Device does not support the requested format " << format);
#endif
		throw std::exception("RenderTexture");
	}
	// Initialize the render target texture description.
	ZeroMemory(&m_textureDesc, sizeof(m_textureDesc));
	// Setup the render target texture description.
	m_textureDesc.Width = width;
	m_textureDesc.Height = height;
	m_textureDesc.MipLevels = 1;
	m_textureDesc.ArraySize = 1;
	m_textureDesc.Format = format;
	m_textureDesc.SampleDesc.Count = 1;
	m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	if (useWithShader)
	{
		m_textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	}
	else
	{
		m_textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	}
	m_textureDesc.CPUAccessFlags = 0;
	m_textureDesc.MiscFlags = 0;
	// Create the render target textures.
	DX::ThrowIfFailed(device->CreateTexture2D(&m_textureDesc, nullptr, m_texture2D.ReleaseAndGetAddressOf()));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	// Initialize the render target view description.
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	// Create the render target view.
	DX::ThrowIfFailed(device->CreateRenderTargetView(m_texture2D.Get(), &renderTargetViewDesc, m_renderTargetView.ReleaseAndGetAddressOf()));


	if (useWithShader)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		// Initialize the render target texture description.
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		// Setup the description of the shader resource view.
		shaderResourceViewDesc.Format = format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		// Create the shader resource views.
		DX::ThrowIfFailed(device->CreateShaderResourceView(m_texture2D.Get(), &shaderResourceViewDesc, m_shaderResourceView.ReleaseAndGetAddressOf()));
	}

}