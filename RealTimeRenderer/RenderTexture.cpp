#include "pch.h"
#include "RenderTexture.h"

RenderTexture::RenderTexture(ID3D11Device1* device, DXGI_FORMAT format, UINT width, UINT height, bool useWithShader)
	: m_useWithShader(useWithShader)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	if (useWithShader)
	{
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	}
	else
	{
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	}
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	SetTextureDesc(textureDesc);

	// Create the render target textures.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	DX::ThrowIfFailed(device->CreateTexture2D(&textureDesc, nullptr, texture2D.ReleaseAndGetAddressOf()));
	SetTexture2D(texture2D);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	// Initialize the render target view description.
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	// Create the render target view.
	DX::ThrowIfFailed(device->CreateRenderTargetView(texture2D.Get(), &renderTargetViewDesc, m_renderTargetView.ReleaseAndGetAddressOf()));


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
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		DX::ThrowIfFailed(device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, shaderResourceView.ReleaseAndGetAddressOf()));
		SetShaderResourceView(shaderResourceView);
	}

}