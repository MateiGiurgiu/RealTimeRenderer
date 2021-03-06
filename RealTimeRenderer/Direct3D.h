﻿//
// Direct3D.h - A wrapper for the Direct3D 11 device and swapchain
//

#pragma once
#include "RenderTexture.h"

namespace DX
{
	const int BUFFER_COUNT = 3;

    // Provides an interface for an application that owns Direct3D to be notified of the device being lost or created.
    interface IDeviceNotify
    {
        virtual void OnDeviceLost() = 0;
        virtual void OnDeviceRestored() = 0;

		virtual ~IDeviceNotify() = default;
    };

    // Controls all the DirectX device resources.
    class Direct3D
    {
    public:
        static const unsigned int c_FlipPresent     = 0x1;
        static const unsigned int c_AllowTearing    = 0x2;
        static const unsigned int c_EnableHDR       = 0x4;

        explicit Direct3D(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
                        DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
                        UINT backBufferCount = 2,
                        D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_10_0,
                        unsigned int flags = c_FlipPresent) noexcept;

		Direct3D(const Direct3D& other) = default;
		Direct3D& operator=(const Direct3D& other) = default;

        void CreateDeviceResources();
        void CreateWindowSizeDependentResources();
        void SetWindow(HWND window, int width, int height);
        bool WindowSizeChanged(int width, int height);
        void HandleDeviceLost();
        void RegisterDeviceNotify(IDeviceNotify* const deviceNotify) { m_deviceNotify = deviceNotify; }
		void SetGBufferAsRenderTarget();
		void ClearGBuffers();
		void SetBackBufferAsRenderTarget();
		void ClearBackBuffer();
		void ClearDepthStencil();
		void SetShadowMapAsRenderTarget();
		void ClearShadowMap();
        void Present();

        // Device Accessors.
        const RECT& GetOutputSize() const { return m_outputSize; }

        // Direct3D Accessors.
        ID3D11Device1*          GetD3DDevice() const                    { return m_d3dDevice.Get(); }
        ID3D11DeviceContext1*   GetD3DDeviceContext() const             { return m_d3dContext.Get(); }
        IDXGISwapChain1*        GetSwapChain() const                    { return m_swapChain.Get(); }
        D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const           { return m_d3dFeatureLevel; }
        ID3D11Texture2D*        GetBackBufferRenderTarget() const       { return m_backBufferRenderTarget.Get(); }
        ID3D11Texture2D*        GetDepthStencil() const                 { return m_depthStencilRenderTarget.Get(); }
        ID3D11RenderTargetView*	GetBackBufferRenderTargetView() const   { return m_backBufferRenderTargetView.Get(); }
        ID3D11DepthStencilView* GetDepthStencilView() const             { return m_depthStencilRenderTargetView.Get(); }
        DXGI_FORMAT             GetBackBufferFormat() const             { return m_backBufferFormat; }
        DXGI_FORMAT             GetDepthBufferFormat() const            { return m_depthBufferFormat; }
        const D3D11_VIEWPORT&   GetScreenViewport() const               { return m_screenViewport; }
        UINT                    GetBackBufferCount() const              { return m_backBufferCount; }
        DXGI_COLOR_SPACE_TYPE   GetColorSpace() const                   { return m_colorSpace; }
        unsigned int            GetDeviceOptions() const                { return m_options; }
		UINT					GetScreenWidth() const					{ return m_Width; }
		UINT					GetScreenHeight() const					{ return m_Height; }

        // Performance events
        void PIXBeginEvent(_In_z_ const wchar_t* const name)
        {
            m_d3dAnnotation->BeginEvent(name);
        }

        void PIXEndEvent()
        {
            m_d3dAnnotation->EndEvent();
        }

        void PIXSetMarker(_In_z_ const wchar_t* const name)
        {
            m_d3dAnnotation->SetMarker(name);
        }

		std::shared_ptr<RenderTexture> GetColorBuffer() const { return m_gBufferColor; }
		std::shared_ptr<RenderTexture> GetNormalBuffer() const { return m_gBufferNormals; }
		std::shared_ptr<RenderTexture> GetPositionBuffer() const { return m_gBufferPos; }
		std::shared_ptr<RenderTexture> GetShadowMap() const { return m_shadowMap; }

    private:
        void CreateFactory();
        void GetHardwareAdapter(IDXGIAdapter1** ppAdapter);
        void UpdateColorSpace();

        // Direct3D objects.
        Microsoft::WRL::ComPtr<IDXGIFactory2>               m_dxgiFactory;
        Microsoft::WRL::ComPtr<ID3D11Device1>               m_d3dDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext1>        m_d3dContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain1>             m_swapChain;
        Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>   m_d3dAnnotation;

        // Direct3D rendering objects. Required for 3D.
		std::shared_ptr<RenderTexture>						m_gBufferColor;
		std::shared_ptr<RenderTexture>						m_gBufferNormals;
		std::shared_ptr<RenderTexture>						m_gBufferPos;
		std::shared_ptr<RenderTexture>						m_shadowMap;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_depthStencilRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_depthStencilRenderTargetView;

        Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_backBufferRenderTarget;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_backBufferRenderTargetView;
        D3D11_VIEWPORT										m_screenViewport;

        // Direct3D properties.
        DXGI_FORMAT											m_backBufferFormat;
        DXGI_FORMAT											m_depthBufferFormat;
        UINT												m_backBufferCount;
        D3D_FEATURE_LEVEL									m_d3dMinFeatureLevel;

        // Cached device properties.
        HWND												m_window;
        D3D_FEATURE_LEVEL									m_d3dFeatureLevel;
        RECT												m_outputSize;

        // HDR Support
        DXGI_COLOR_SPACE_TYPE								m_colorSpace;

        // Direct3D options (see flags above)
        unsigned int										m_options;

        // The IDeviceNotify can be held directly as it owns the Direct3D.
        IDeviceNotify*										m_deviceNotify;

		// rendering size
		UINT												m_Width;
		UINT												m_Height;
    };
}
