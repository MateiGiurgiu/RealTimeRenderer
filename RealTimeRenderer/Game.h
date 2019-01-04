//
// Game.h
//

#pragma once

#include "Direct3D.h"
#include "StepTimer.h"
#include "MeshRenderer.h"
#include <d3dx11effect.h>
#include <Keyboard.h>
#include "Camera.h"
#include <AntTweakBar.h>
#include <vector>
#include "IRenderable.h"
#include "RenderQuad.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

	void CreateGameObjects();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::Direct3D> m_Direct3D;

    // Rendering loop timer.
    DX::StepTimer m_timer;

	// Input
	std::unique_ptr<DirectX::Keyboard> m_keyboard;

	// RenderQuad for doing post processing
	std::unique_ptr<RenderQuad> m_renderQuad;

	std::shared_ptr<MeshRenderer> m_meshRenderer;
	std::vector<std::shared_ptr<IRenderable>> m_renderables;


	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_proj;
	DirectX::SimpleMath::Vector4 m_LightPos;

	Camera m_camera;
	DirectX::SimpleMath::Vector4* instanceData;

	// GUI
	TwBar* infoBar = nullptr;
	ID3DX11EffectVectorVariable* m_lightPosVariable;
};