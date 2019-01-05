﻿//
// Direct3D.cpp - A wrapper for the Direct3D 11 device and swapchain
//                       (requires DirectX 11.1 Runtime)
//

#include "pch.h"
#include "Direct3D.h"

using namespace DirectX;
using namespace DX;

using Microsoft::WRL::ComPtr;

namespace
{
#if defined(_DEBUG)
    // Check for SDK Layer support.
    inline bool SdkLayersAvailable()
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
            0,
            D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
            nullptr,                    // Any feature level will do.
            0,
            D3D11_SDK_VERSION,
            nullptr,                    // No need to keep the D3D device reference.
            nullptr,                    // No need to know the feature level.
            nullptr                     // No need to keep the D3D device context reference.
            );

        return SUCCEEDED(hr);
    }
#endif

    inline DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt)
    {
        switch (fmt)
        {
	        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
	        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
	        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
	        default:                                return fmt;
        }
    }
};

// Constructor for Direct3D.
Direct3D::Direct3D(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, UINT backBufferCount, D3D_FEATURE_LEVEL minFeatureLevel, unsigned int flags) 
	noexcept :
        m_screenViewport{},
        m_backBufferFormat(backBufferFormat),
        m_depthBufferFormat(depthBufferFormat),
        m_backBufferCount(backBufferCount),
        m_d3dMinFeatureLevel(minFeatureLevel),
        m_window(nullptr),
        m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
        m_outputSize{0, 0, 1, 1},
        m_colorSpace(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709),
        m_options(flags | c_FlipPresent),
        m_deviceNotify(nullptr)
{
	 // empty
}

// Configures the Direct3D device, and stores handles to it and the device context.
void Direct3D::CreateDeviceResources() 
{
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    if (SdkLayersAvailable())
    {
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
    else
    {
        OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
    }
#endif

    CreateFactory();

    // Determines whether tearing support is available for fullscreen borderless windows.
    if (m_options & c_AllowTearing)
    {
        BOOL allowTearing = FALSE;

        ComPtr<IDXGIFactory5> factory5;
        HRESULT hr = m_dxgiFactory.As(&factory5);
        if (SUCCEEDED(hr))
        {
            hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
        }

        if (FAILED(hr) || !allowTearing)
        {
            m_options &= ~c_AllowTearing;
#ifdef _DEBUG
            OutputDebugStringA("WARNING: Variable refresh rate displays not supported");
#endif
        }
    }

    // Disable HDR if we are on an OS that can't support FLIP swap effects
    if (m_options & c_EnableHDR)
    {
        ComPtr<IDXGIFactory5> factory5;
        if (FAILED(m_dxgiFactory.As(&factory5)))
        {
            m_options &= ~c_EnableHDR;
#ifdef _DEBUG
            OutputDebugStringA("WARNING: HDR swap chains not supported");
#endif
        }
    }

    // Disable FLIP if not on a supporting OS
    if (m_options & c_FlipPresent)
    {
        ComPtr<IDXGIFactory4> factory4;
        if (FAILED(m_dxgiFactory.As(&factory4)))
        {
            m_options &= ~c_FlipPresent;
#ifdef _DEBUG
            OutputDebugStringA("INFO: Flip swap effects not supported");
#endif
        }
    }

    // Determine DirectX hardware feature levels this app will support.
    static const D3D_FEATURE_LEVEL s_featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    UINT featLevelCount = 0;
    for (; featLevelCount < _countof(s_featureLevels); ++featLevelCount)
    {
        if (s_featureLevels[featLevelCount] < m_d3dMinFeatureLevel)
            break;
    }

    if (!featLevelCount)
    {
        throw std::out_of_range("minFeatureLevel too high");
    }

    ComPtr<IDXGIAdapter1> adapter;
    GetHardwareAdapter(adapter.GetAddressOf());

    // Create the Direct3D 11 API device object and a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    HRESULT hr = E_FAIL;
    if (adapter)
    {
        hr = D3D11CreateDevice(
            adapter.Get(),
            D3D_DRIVER_TYPE_UNKNOWN,
            0,
            creationFlags,
            s_featureLevels,
            featLevelCount,
            D3D11_SDK_VERSION,
            device.GetAddressOf(),  // Returns the Direct3D device created.
            &m_d3dFeatureLevel,     // Returns feature level of device created.
            context.GetAddressOf()  // Returns the device immediate context.
            );
    }
#if defined(NDEBUG)
    else
    {
        throw std::exception("No Direct3D hardware device found");
    }
#else
    if (FAILED(hr))
    {
        // If the initialization fails, fall back to the WARP device.
        // For more information on WARP, see: 
        // http://go.microsoft.com/fwlink/?LinkId=286690
        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
            0,
            creationFlags,
            s_featureLevels,
            featLevelCount,
            D3D11_SDK_VERSION,
            device.GetAddressOf(),
            &m_d3dFeatureLevel,
            context.GetAddressOf()
            );

        if (SUCCEEDED(hr))
        {
            OutputDebugStringA("Direct3D Adapter - WARP\n");
        }
    }
#endif

    ThrowIfFailed(hr);

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				D3D11_MESSAGE_ID_DEVICE_IASETVERTEXBUFFERS_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_IASETINDEXBUFFER_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_VSSETCONSTANTBUFFERS_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_GSSETCONSTANTBUFFERS_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_PSSETCONSTANTBUFFERS_HAZARD,
				D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
				D3D11_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDFORMAT,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDSAMPLES,
				D3D11_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDUSAGE,
				D3D11_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDBINDFLAGS,
				D3D11_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDCPUACCESSFLAGS,
				D3D11_MESSAGE_ID_CREATEBUFFER_UNRECOGNIZEDMISCFLAGS,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDCPUACCESSFLAGS,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDBINDFLAGS,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDINITIALDATA,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDDIMENSIONS,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDMIPLEVELS,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDMISCFLAGS,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDARG_RETURN,
				D3D11_MESSAGE_ID_CREATEBUFFER_OUTOFMEMORY_RETURN,
				D3D11_MESSAGE_ID_CREATEBUFFER_NULLDESC,
				D3D11_MESSAGE_ID_CREATEBUFFER_INVALIDCONSTANTBUFFERBINDINGS,
				D3D11_MESSAGE_ID_CREATEBUFFER_LARGEALLOCATION,
				D3D11_MESSAGE_ID_VSSETSHADER_UNBINDDELETINGOBJECT,
				D3D11_MESSAGE_ID_VSSETSHADERRESOURCES_UNBINDDELETINGOBJECT
            };

			D3D11_MESSAGE_CATEGORY hideCategory[] =
			{
				 D3D11_MESSAGE_CATEGORY_SHADER
			};

            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
			filter.DenyList.NumCategories = _countof(hideCategory);
			filter.DenyList.pCategoryList = hideCategory;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    ThrowIfFailed(device.As(&m_d3dDevice));
    ThrowIfFailed(context.As(&m_d3dContext));
    ThrowIfFailed(context.As(&m_d3dAnnotation));
}

// These resources need to be recreated every time the window size is changed.
void Direct3D::CreateWindowSizeDependentResources() 
{
    if (!m_window)
    {
        throw std::exception("Call SetWindow with a valid Win32 window handle");
    }

    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = {nullptr};
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_backBufferRenderTargetView.Reset();
    m_d3dDepthStencilView.Reset();
	m_backBufferRenderTarget.Reset();
    m_depthStencil.Reset();
    m_d3dContext->Flush();

    // Determine the render target size in pixels.
    UINT backBufferWidth = std::max<UINT>(m_outputSize.right - m_outputSize.left, 1);
    UINT backBufferHeight = std::max<UINT>(m_outputSize.bottom - m_outputSize.top, 1);
    DXGI_FORMAT backBufferFormat = (m_options & (c_FlipPresent | c_AllowTearing | c_EnableHDR)) ? NoSRGB(m_backBufferFormat) : m_backBufferFormat;

	m_Width = backBufferWidth;
	m_Height = backBufferHeight;

    if (m_swapChain)
    {
        // If the swap chain already exists, resize it.
        HRESULT hr = m_swapChain->ResizeBuffers(
            m_backBufferCount,
            backBufferWidth,
            backBufferHeight,
            backBufferFormat,
            (m_options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0
            );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
#ifdef _DEBUG
            char buff[64] = {};
            sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
            OutputDebugStringA(buff);
#endif
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            HandleDeviceLost();

            // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            ThrowIfFailed(hr);
        }
    }
    else
    {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = m_backBufferCount;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = (m_options & (c_FlipPresent | c_AllowTearing | c_EnableHDR)) ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = (m_options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr, m_swapChain.ReleaseAndGetAddressOf()
            ));

        // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
        ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Handle color space settings for HDR
    UpdateColorSpace();

	// Create a render target view of the swap chain back buffer.
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_backBufferRenderTarget.ReleaseAndGetAddressOf())));

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat);
	ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(
		m_backBufferRenderTarget.Get(),
		&renderTargetViewDesc,
		m_backBufferRenderTargetView.ReleaseAndGetAddressOf()
	));
	

	// DEPTH & STENCIL
    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        // Create a depth stencil view for use with 3D rendering if needed.
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            m_depthBufferFormat,
            backBufferWidth,
            backBufferHeight,
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL
            );

        ThrowIfFailed(m_d3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            m_depthStencil.ReleaseAndGetAddressOf()
            ));

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
        ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(
            m_depthStencil.Get(),
            &depthStencilViewDesc,
            m_d3dDepthStencilView.ReleaseAndGetAddressOf()
            ));
    }
    
    // Set the 3D rendering viewport to target the entire window.
    m_screenViewport = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(backBufferWidth), static_cast<float>(backBufferHeight));

	CreateTestBuffers();
}

// This method creates the multiple render targets needed for deferred rendering
void Direct3D::CreateDeferredBuffers()
{
	D3D11_TEXTURE2D_DESC renderTextureDesc;
	// Initialize the render target texture description.
	ZeroMemory(&renderTextureDesc, sizeof(renderTextureDesc));
	// Setup the render target texture description.
	renderTextureDesc.Width = m_Width;
	renderTextureDesc.Height = m_Height;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;
	// Create the render target textures.
	for (unsigned int i = 0; i < BUFFER_COUNT; i++)
	{
		DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&renderTextureDesc, nullptr, &m_deferredRenderTargets[i]));
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	// Initialize the render target view description.
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = renderTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	// Create the render target views.
	for (unsigned int i = 0; i < BUFFER_COUNT; i++)
	{
		DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(m_deferredRenderTargets[i].Get(), &renderTargetViewDesc, &m_deferredRenderTargetsView[i]));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	// Initialize the render target texture description.
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = renderTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	// Create the shader resource views.
	for (unsigned int i = 0; i < BUFFER_COUNT; i++)
	{
		DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(m_deferredRenderTargets[i].Get(), &shaderResourceViewDesc, m_deferredRenderShaderResource[i].ReleaseAndGetAddressOf()));
	}

	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthStencilTextureDesc, sizeof(depthStencilTextureDesc));
	// Set up the description of the depth buffer.
	depthStencilTextureDesc.Width = m_Width;
	depthStencilTextureDesc.Height = m_Height;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.SampleDesc.Count = 1;
	depthStencilTextureDesc.SampleDesc.Quality = 0;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.MiscFlags = 0;
	// Create the texture for the depth buffer using the filled out description.
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilTextureDesc, nullptr, m_depthStencilRenderTarget.ReleaseAndGetAddressOf()));



	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Initialize the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	// Create the depth stencil view.
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(m_depthStencilRenderTarget.Get(), &depthStencilViewDesc, m_depthStencilRenderTargetView.ReleaseAndGetAddressOf()));
}

void Direct3D::CreateTestBuffers()
{
	customRenderTexture = new RenderTexture(m_d3dDevice.Get(), m_backBufferFormat, m_Width, m_Height);
	customRenderTexture2 = new RenderTexture(m_d3dDevice.Get(), m_backBufferFormat, m_Width, m_Height);
}

ID3D11RenderTargetView* const* Direct3D::GetBuffers()
{
	temp[0] = customRenderTexture->GetRenderTargetView().Get();
	temp[1] = customRenderTexture2->GetRenderTargetView().Get();
	return temp;
}

// This method is called when the Win32 window is created (or re-created).
void Direct3D::SetWindow(HWND window, int width, int height)
{
    m_window = window;

    m_outputSize.left = m_outputSize.top = 0;
    m_outputSize.right = width;
    m_outputSize.bottom = height;
}

// This method is called when the Win32 window changes size
bool Direct3D::WindowSizeChanged(int width, int height)
{
    RECT newRc;
    newRc.left = newRc.top = 0;
    newRc.right = width;
    newRc.bottom = height;
    if (newRc == m_outputSize)
    {
        // Handle color space settings for HDR
        UpdateColorSpace();

        return false;
    }

    m_outputSize = newRc;
    CreateWindowSizeDependentResources();
    return true;
}

// Recreate all device resources and set them back to the current state.
void Direct3D::HandleDeviceLost()
{
    if (m_deviceNotify)
    {
        m_deviceNotify->OnDeviceLost();
    }

    m_d3dDepthStencilView.Reset();
	m_backBufferRenderTargetView.Reset();
	m_backBufferRenderTarget.Reset();
    m_depthStencil.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dAnnotation.Reset();

#ifdef _DEBUG
    {
        ComPtr<ID3D11Debug> d3dDebug;
        if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
        {
            d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
        }
    }
#endif

    m_d3dDevice.Reset();
    m_dxgiFactory.Reset();

    CreateDeviceResources();
    CreateWindowSizeDependentResources();

    if (m_deviceNotify)
    {
        m_deviceNotify->OnDeviceRestored();
    }
}

// Present the contents of the swap chain to the screen.
void Direct3D::Present() 
{
    HRESULT hr;
    if (m_options & c_AllowTearing)
    {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        hr = m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    }
    else
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        hr = m_swapChain->Present(1, 0);
    }

    // Discard the contents of the render target.
    // This is a valid operation only when the existing contents will be entirely
    // overwritten. If dirty or scroll rects are used, this call should be removed.
    m_d3dContext->DiscardView(m_backBufferRenderTargetView.Get());

    if (m_d3dDepthStencilView)
    {
        // Discard the contents of the depth stencil.
        m_d3dContext->DiscardView(m_d3dDepthStencilView.Get());
    }

    // If the device was removed either by a disconnection or a driver upgrade, we 
    // must recreate all device resources.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
#ifdef _DEBUG
        char buff[64] = {};
        sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
        OutputDebugStringA(buff);
#endif
        HandleDeviceLost();
    }
    else
    {
        ThrowIfFailed(hr);

        if (!m_dxgiFactory->IsCurrent())
        {
            // Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
            CreateFactory();
        }
    }
}

void Direct3D::CreateFactory()
{
#if defined(_DEBUG) && (_WIN32_WINNT >= 0x0603 /*_WIN32_WINNT_WINBLUE*/)
    bool debugDXGI = false;
    {
        ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
        {
            debugDXGI = true;

            ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
        }
    }

    if (!debugDXGI)
#endif

    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
}

// This method acquires the first available hardware adapter.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
void Direct3D::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

#if defined(__dxgi1_6_h__) && defined(NTDDI_WIN10_RS4)
    ComPtr<IDXGIFactory6> factory6;
    HRESULT hr = m_dxgiFactory.As(&factory6);
    if (SUCCEEDED(hr))
    {
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())); adapterIndex++)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                continue;
            }

        #ifdef _DEBUG
            wchar_t buff[256] = {};
            swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
            OutputDebugStringW(buff);
        #endif

            break;
        }
    }
    else
#endif
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf()); adapterIndex++)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // Don't select the Basic Render Driver adapter.
            continue;
        }

#ifdef _DEBUG
        wchar_t buff[256] = {};
        swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
        OutputDebugStringW(buff);
#endif

        break;
    }

    *ppAdapter = adapter.Detach();
}

// Sets the color space for the swap chain in order to handle HDR output.
void Direct3D::UpdateColorSpace()
{
    DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

    bool isDisplayHDR10 = false;

#if defined(NTDDI_WIN10_RS2)
    if (m_swapChain)
    {
        ComPtr<IDXGIOutput> output;
        if (SUCCEEDED(m_swapChain->GetContainingOutput(output.GetAddressOf())))
        {
            ComPtr<IDXGIOutput6> output6;
            if (SUCCEEDED(output.As(&output6)))
            {
                DXGI_OUTPUT_DESC1 desc;
                ThrowIfFailed(output6->GetDesc1(&desc));

                if (desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
                {
                    // Display output is HDR10.
                    isDisplayHDR10 = true;
                }
            }
        }
    }
#endif

    if ((m_options & c_EnableHDR) && isDisplayHDR10)
    {
        switch (m_backBufferFormat)
        {
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            // The application creates the HDR10 signal.
            colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
            break;

        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            // The system creates the HDR10 signal; application uses linear values.
            colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
            break;

        default:
            break;
        }
    }

    m_colorSpace = colorSpace;

    ComPtr<IDXGISwapChain3> swapChain3;
    if (SUCCEEDED(m_swapChain.As(&swapChain3)))
    {
        UINT colorSpaceSupport = 0;
        if (SUCCEEDED(swapChain3->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport))
            && (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
        {
            ThrowIfFailed(swapChain3->SetColorSpace1(colorSpace));
        }
    }
}
