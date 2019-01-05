#include "pch.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Geometry.h"
#include "Texture.h"

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

	CreateGameObjects();

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
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
	float time = float(timer.GetTotalSeconds());

	m_camera.Update(elapsedTime);
	for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
	{
		m_gameObjects[i]->Update(elapsedTime);
	}
	m_gameObjects[0]->SetOrientation(0, time / 2, 0);
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

    Clear();

    m_Direct3D->PIXBeginEvent(L"Render");

	ID3D11DeviceContext1* context = m_Direct3D->GetD3DDeviceContext();
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

    // TODO: Add your rendering code here.
	m_lightPosVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightPos));

	for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
	{
		m_gameObjects[i]->Render(context, m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix());
	}

    m_Direct3D->PIXEndEvent();


	// POST PROCESSING
	m_Direct3D->PIXBeginEvent(L"PostProcessing");

	m_Direct3D->SetBackBufferAsRenderTarget();

	m_renderQuad->GetShader()->SetTexture("buffer1", m_Direct3D->m_gBufferColor->GetShaderResourceView());
	m_renderQuad->GetShader()->SetTexture("buffer2", m_Direct3D->m_gBufferNormals->GetShaderResourceView());
	m_renderQuad->GetShader()->SetTexture("buffer3", m_Direct3D->m_gBufferPos->GetShaderResourceView());
	m_renderQuad->Draw(context, m_currentVisualizationType);

	m_Direct3D->PIXEndEvent();

	// Draw the GUI
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

	m_Direct3D->ClearGBuffers();
	m_Direct3D->ClearBackBuffer();
	m_Direct3D->ClearDepthStencil();

	m_Direct3D->SetGBufferAsRenderTarget();

	//context->ClearRenderTargetView(m_Direct3D->customRenderTexture->GetRenderTargetView().Get(), Colors::CornflowerBlue);
	//context->ClearRenderTargetView(m_Direct3D->customRenderTexture2->GetRenderTargetView().Get(), Colors::CornflowerBlue);
	//context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//context->OMSetRenderTargets(2, m_Direct3D->GetBuffers(), depthStencil);

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
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

	std::shared_ptr<Shader> shader = ResourceManager::GetShader(L"Shaders/SimpleShader2.fx", device);

	m_lightPosVariable = shader->GetEffect()->GetVariableByName("LightPos")->AsVector();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

	// Render quad, very important for post processing
	std::shared_ptr<Shader> postProcessingShader = ResourceManager::GetShader(L"Shaders/PP_Test.fx", device);
	m_renderQuad = std::make_unique<RenderQuad>(device, postProcessingShader, m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height);;

	m_camera = Camera(SimpleMath::Vector3(0.f, 0.0f, -4.f), m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height, 55.0f);

	m_LightPos = SimpleMath::Vector4(0, 4, 0, 1);

	// Initialize GUI
	TwInit(TW_DIRECT3D11, device);
	TwWindowSize(static_cast<int>(m_Direct3D->GetScreenViewport().Width), static_cast<int>(m_Direct3D->GetScreenViewport().Height));

	// Make the widget bar
	infoBar = TwNewBar("NameOfMyTweakBar");

	// Add GUI widgets
	TwAddVarRW(infoBar, "Camera Movement Speed", TW_TYPE_FLOAT, &m_camera.MovementSpeed, "");
	TwAddVarRW(infoBar, "Camera Rotation Speed", TW_TYPE_FLOAT, &m_camera.RotationSpeed, "");
	TwAddVarRO(infoBar, "MeshesLoaded", TW_TYPE_INT32, &ResourceManager::MeshesLoaded, "");
	TwAddVarRW(infoBar, "Density", TwDefineEnumFromString("Visualization", "Buffer1,Buffer2,Buffer3"), &m_currentVisualizationType, nullptr);
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

	m_gameObjects.reserve(10);
	auto geom = std::make_shared<Geometry>(device, L"Models/SpaceShip.sdkmesh", L"Shaders/SimpleShader2.fx");

	m_diffuse = ResourceManager::GetTexture(L"Textures/hytale.jpg", device);
	geom->SetDiffuseTexture(m_diffuse);
	geom->SetPosition(4.0, -2, 4);
	m_gameObjects.push_back(geom);

}
