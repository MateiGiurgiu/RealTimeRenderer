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
#include "SceneNode.h"
#include "Skybox.h"
#include "DirectionalLight.h"

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

	// Game Objects list
	std::vector<std::shared_ptr<SceneNode>> m_gameObjects;

	// Lights
	std::shared_ptr<DirectionalLight> m_directionalLight;

	float lightPosX = -4;
	float lightPosY = 3;

	// Skybox
	std::unique_ptr<Skybox> m_skybox;

	bool rotate = true;
	Camera m_camera;
	DirectX::SimpleMath::Vector4* instanceData;

	// GUI
	TwBar* infoBar = nullptr;
	int m_currentVisualizationType = 5;
};