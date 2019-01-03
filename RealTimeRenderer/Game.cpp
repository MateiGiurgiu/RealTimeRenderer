#include "pch.h"
#include "Game.h"
#include "ResourceManager.h"

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

	CreateGameObjects();

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
	x = 54;
	infoBar = TwNewBar("NameOfMyTweakBar");
	TwAddVarRW(infoBar, "NameOfMyVariable", TW_TYPE_INT32, &x, "");

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
	//m_world = SimpleMath::Matrix::CreateRotationZ(time);
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

	m_meshRenderer->Draw(context, m_world, m_camera.GetViewMatrix(), m_proj);

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
	auto renderTarget = m_Direct3D->GetRenderTargetView();
	auto depthStencil = m_Direct3D->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

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

	std::shared_ptr<Mesh> mesh = ResourceManager::GetMesh(L"Models/Axis.sdkmesh", device);
	std::shared_ptr<Shader> shader = ResourceManager::GetShader(L"Shaders/SimpleShader2.fx", device);
	m_meshRenderer = std::make_shared<MeshRenderer>(mesh, shader, device);

	m_lightPosVariable = shader->GetEffect()->GetVariableByName("LightPos")->AsVector();
	m_world = SimpleMath::Matrix::Identity;
	//m_world = SimpleMath::Matrix::CreateScale(1.0f);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

	TwInit(TW_DIRECT3D11, device);
	TwWindowSize(m_Direct3D->GetScreenViewport().Width, m_Direct3D->GetScreenViewport().Height);

	m_camera = Camera(SimpleMath::Vector3(0.f, 0.0f, -4.f));

	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(m_Direct3D->GetScreenViewport().Width) / float(m_Direct3D->GetScreenViewport().Height), 0.1f, 100.f);

	m_LightPos = SimpleMath::Vector4(0, 4, 0, 1);
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

}
