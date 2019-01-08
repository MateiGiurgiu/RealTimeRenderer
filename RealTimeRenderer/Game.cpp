#include "pch.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Geometry.h"
#include "Texture.h"
#include "VoxelTerrain.h"
#include "ParticleSystem.h"
#include "Rocket.h"

extern void ExitGame();

using namespace DirectX;

using Microsoft::WRL::ComPtr;


Game::Game() noexcept(false)
{
    m_Direct3D = std::make_unique<DX::Direct3D>();
    m_Direct3D->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_Direct3D->SetWindow(window, width, height);

    m_Direct3D->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_Direct3D->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
	
	

	// Create keyboard singleton
	m_keyboard = std::make_unique<Keyboard>();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float deltaTime = float(timer.GetElapsedSeconds());
	float currentTime = float(timer.GetTotalSeconds());
	m_fps = timer.GetFramesPerSecond();
	m_meshesUsed = ResourceManager::GetMeshesLoaded();
	m_shadersUsed = ResourceManager::GetShadersLoaded();
	m_texturesUsed = ResourceManager::GetTexturesLoaded();

	if (Keyboard::Get().GetState().R)
	{
		m_terrain->Reset();
		m_rocket->Reset();
		m_cameras[0]->Reset();
		m_cameras[1]->Reset();
	}

	if (Keyboard::Get().GetState().F1)
	{
		m_activeCamera = 0;
	}
	if (Keyboard::Get().GetState().F2)
	{
		m_activeCamera = 1;
	}
	if (Keyboard::Get().GetState().F3)
	{
		m_activeCamera = 2;
	}
	if (Keyboard::Get().GetState().F4)
	{
		m_activeCamera = 3;
	}
	if (Keyboard::Get().GetState().F5)
	{
		m_activeCamera = 4;
	}

	m_cameras[m_activeCamera]->Update(deltaTime);

	for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
	{
		m_gameObjects[i]->Update(deltaTime, currentTime);
	}

	//m_gameObjects[0]->SetPosition(m_directionalLight->GetPosition());
	//m_gameObjects[0]->SetOrientation(m_directionalLight->GetEulerAngles());
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

	ID3D11DeviceContext1* context = m_Direct3D->GetD3DDeviceContext();
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

	//--------------------------------------------------------------------------------------
	// Clear ALL buffers
	//--------------------------------------------------------------------------------------
    Clear();

	//--------------------------------------------------------------------------------------
	// Render Shadows
	//--------------------------------------------------------------------------------------
	m_Direct3D->PIXBeginEvent(L"Render Shadows");
	{
		m_Direct3D->SetShadowMapAsRenderTarget();
		for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
		{
			m_gameObjects[i]->RenderShadow(context, m_directionalLight->GetViewMatrix(), m_directionalLight->GetProjectionMatrix());
		}
		m_Direct3D->ClearDepthStencil();
	}
	m_Direct3D->PIXEndEvent();


	//--------------------------------------------------------------------------------------
	// Deferred Rendering
	//--------------------------------------------------------------------------------------
    m_Direct3D->PIXBeginEvent(L"Deferred Rendering");
	{
		m_Direct3D->SetGBufferAsRenderTarget();

		for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
		{
			m_gameObjects[i]->RenderDeferred(context, m_cameras[m_activeCamera]->GetViewMatrix(), m_cameras[m_activeCamera]->GetProjectionMatrix());
		}
	}
    m_Direct3D->PIXEndEvent();


	//--------------------------------------------------------------------------------------
	// Deferred Lighting
	//--------------------------------------------------------------------------------------
	m_Direct3D->PIXBeginEvent(L"Deferred Lighting");
	{
		m_Direct3D->SetBackBufferAsRenderTarget();

		m_renderQuad->GetShader()->SetVector("ViewDir", m_cameras[m_activeCamera]->GetViewDirection());
		m_renderQuad->GetShader()->SetColor("AmbientColor", SimpleMath::Color(0.039f, 0.815f, 1.0f, 1.0f));

		m_renderQuad->GetShader()->SetVector("PointLightPosition", m_rocket->GetLightPosition());
		m_renderQuad->GetShader()->SetColor("PointLightColor", m_rocket->GetLightColor());

		m_renderQuad->GetShader()->SetVector("DirectionalLightPos", m_directionalLight->GetPosition());
		m_renderQuad->GetShader()->SetVector("DirectionalLightDir", m_directionalLight->GetLightDir());
		m_renderQuad->GetShader()->SetColor("DirectionalLightColor", m_directionalLight->GetLightColor());
		m_renderQuad->GetShader()->SetMatrix("DirectionalLightView", m_directionalLight->GetViewMatrix());
		m_renderQuad->GetShader()->SetMatrix("DirectionalLightProj", m_directionalLight->GetProjectionMatrix());

		m_renderQuad->GetShader()->SetTexture("bufferColor", m_Direct3D->GetColorBuffer()->GetShaderResourceView());
		m_renderQuad->GetShader()->SetTexture("bufferNormal", m_Direct3D->GetNormalBuffer()->GetShaderResourceView());
		m_renderQuad->GetShader()->SetTexture("bufferPosition", m_Direct3D->GetPositionBuffer()->GetShaderResourceView());
		m_renderQuad->GetShader()->SetTexture("shadowMap", m_Direct3D->GetShadowMap()->GetShaderResourceView());
		m_renderQuad->GetShader()->SetTexture("toonRamp", *ResourceManager::GetTexture(L"Textures/ToonRamp.png", device));
		m_renderQuad->Draw(context, m_currentVisualizationType);
	}
	m_Direct3D->PIXEndEvent();

	//--------------------------------------------------------------------------------------
	// Skybox
	//--------------------------------------------------------------------------------------
	m_skybox->RenderForward(context, m_cameras[m_activeCamera]->GetViewMatrix(), m_cameras[m_activeCamera]->GetProjectionMatrix());

	//--------------------------------------------------------------------------------------
	// Forward Rendering
	//--------------------------------------------------------------------------------------
	m_Direct3D->PIXBeginEvent(L"Forward Rendering");
	{
		m_Direct3D->SetBackBufferAsRenderTarget();

		for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
		{
			m_gameObjects[i]->RenderForward(context, m_cameras[m_activeCamera]->GetViewMatrix(), m_cameras[m_activeCamera]->GetProjectionMatrix());
		}
	}
	m_Direct3D->PIXEndEvent();

	//--------------------------------------------------------------------------------------
	// GUI
	//--------------------------------------------------------------------------------------
	TwDraw();

    // Show the new frame.
    m_Direct3D->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_Direct3D->PIXBeginEvent(L"Clear");
	
	// Clear the views.
	auto context = m_Direct3D->GetD3DDeviceContext();

	m_Direct3D->ClearShadowMap();
	m_Direct3D->ClearGBuffers();
	m_Direct3D->ClearBackBuffer();
	m_Direct3D->ClearDepthStencil();

	// Set the viewport.
	auto viewport = m_Direct3D->GetScreenViewport();
	context->RSSetViewports(1, &viewport);
	
    m_Direct3D->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_Direct3D->GetOutputSize();
    m_Direct3D->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_Direct3D->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1000;
    height = 800;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	CreateGameObjects();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

	// Render quad, very important for post processing
	m_renderQuad = std::make_unique<RenderQuad>(device, ResourceManager::GetShader(L"Shaders/Deferred.fx", device));

	m_cameras.reserve(5);
	std::shared_ptr<Camera> camera;
	camera = std::make_shared<Camera>(SimpleMath::Vector3(6, 1.0f, -4), m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height, 55.0f);
	m_cameras.push_back(camera);

	camera;
	camera = std::make_shared<Camera>(SimpleMath::Vector3(0, 1.0f, -4), m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height, 55.0f);
	m_cameras.push_back(camera);

	camera;
	camera = std::make_shared<Camera>(SimpleMath::Vector3(-6, 1.0f, -4.f), m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height, 55.0f);
	camera->SetFollowTarget(m_rocket);
	camera->SetDelta(1.8f);
	m_cameras.push_back(camera);

	camera;
	camera = std::make_shared<Camera>(SimpleMath::Vector3(-6, 1.0f, -4.f), m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height, 55.0f);
	camera->SetFollowTarget(m_rocket);
	camera->SetDelta(2.2f);
	m_cameras.push_back(camera);

	camera;
	camera = std::make_shared<Camera>(SimpleMath::Vector3(-6, 1.0f, -4.f), m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height, 55.0f);
	camera->SetFollowTarget(m_rocket);
	camera->SetDelta(0.5f);
	m_cameras.push_back(camera);


	// Initialize GUI
	TwInit(TW_DIRECT3D11, device);
	TwWindowSize(static_cast<int>(m_Direct3D->GetScreenViewport().Width), static_cast<int>(m_Direct3D->GetScreenViewport().Height));

	// Make the widget bar
	infoBar = TwNewBar("Info");

	// Add GUI widgets
	TwAddVarRO(infoBar, "FPS", TW_TYPE_INT32, &m_fps, "");
	TwAddVarRO(infoBar, "Meshes Loaded", TW_TYPE_INT32, &m_meshesUsed, "");
	TwAddVarRO(infoBar, "Shaders Loaded", TW_TYPE_INT32, &m_shadersUsed, "");
	TwAddVarRO(infoBar, "Textures Loaded", TW_TYPE_INT32, &m_texturesUsed, "");
	TwAddVarRW(infoBar, "Density", TwDefineEnumFromString("Visualization", "Color,Specular,Normals,Position,ShadowMap,Light,Toon Light"), &m_currentVisualizationType, nullptr);
}

void Game::OnDeviceLost()
{
	// terminate ant bar
	TwTerminate();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}
#pragma endregion

void Game::CreateGameObjects()
{
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

	m_skybox = std::make_unique<Skybox>(device);
	m_skybox->SetSkyTexture(ResourceManager::GetTexture(L"Textures/env.dds", device));

	m_gameObjects.reserve(10);

	//auto geom = std::make_shared<Geometry>(device, L"Models/Axis.sdkmesh", L"Shaders/Uber.fx");
	//m_gameObjects.push_back(geom);

	m_directionalLight = std::make_shared<DirectionalLight>(55, 0, 0);
	//m_directionalLight->SetLightColor(SimpleMath::Color(1, 1, 1, 1));
	m_gameObjects.push_back(m_directionalLight);

	m_terrain = std::make_shared<VoxelTerrain>(device);
	m_gameObjects.push_back(m_terrain);

	auto enginePS = std::make_shared<ParticleSystem>(device, 1000, ResourceManager::GetShader(L"Shaders/ParticleEngine.fx", device));
	enginePS->SetTexture(ResourceManager::GetTexture(L"Textures/FireParticle.png", device));
	m_gameObjects.push_back(enginePS);

	auto explosionPS = std::make_shared<ParticleSystem>(device, 1000, ResourceManager::GetShader(L"Shaders/ParticleExplosion.fx", device));
	explosionPS->SetTexture(ResourceManager::GetTexture(L"Textures/FireParticle.png", device));
	m_gameObjects.push_back(explosionPS);

	auto pole = std::make_shared<Geometry>(device, L"Models/Pole.sdkmesh", L"Shaders/Uber.fx");
	pole->SetDiffuseTexture(ResourceManager::GetTexture(L"Textures/Pole_D.png", device));
	pole->SetNormalTexture(ResourceManager::GetTexture(L"Textures/Pole_N.png", device));
	pole->SetSpecularTexture(ResourceManager::GetTexture(L"Textures/Pole_S.png", device));
	m_gameObjects.push_back(pole);

	m_rocket = std::make_shared<Rocket>(device, enginePS, explosionPS, pole, m_terrain);
	m_gameObjects.push_back(m_rocket);
}
