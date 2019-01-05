#pragma once
#include "Texture.h"
class RenderTexture : public Texture
{
public:
	RenderTexture(ID3D11Device1* device, DXGI_FORMAT format, UINT width, UINT height, bool useWithShader = true);
	~RenderTexture() override = default;

	bool CanUseWithShader() const { return m_useWithShader; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const { return m_renderTargetView; }

protected:
	bool m_useWithShader;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
};

