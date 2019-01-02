//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <GeometricPrimitive.h>
#include "Mesh.h"
#include <Model.h>
#include <CommonStates.h>
#include <d3dcompiler.h>

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

	m_world = Matrix::CreateRotationZ(time);
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
	m_mesh->PrepareForRendering(context);
	//m_shader->SetShaderParameters(context, m_world, m_view, m_proj, Vector4(0, 5, 0, 1));

	//m_shader->RenderShader(context, m_mesh->GetIndexCount());

	D3DX11_PASS_DESC PassDesc;
	HRESULT r = m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(r))
	{
		LOG_ERROR("Get pass failed");
	}

	r = device->CreateInputLayout(m_mesh->GetLayoutDesc(), m_mesh->GetLayoutDescCount(), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_layout);

	if (FAILED(r))
	{
		LOG_ERROR("Create input layout failed");
	}
	context->IASetInputLayout(m_layout);

	m_pProjectionVariable->SetMatrix(reinterpret_cast<float*>(&m_proj));
	m_pViewVariable->SetMatrix(reinterpret_cast<float*>(&m_view));
	m_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&m_world));
	m_lightPosVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightPos));


	m_pTechnique->GetPassByIndex(0)->Apply(0, context);
	context->DrawIndexed(m_mesh->GetIndexCount(), 0, 0);

    m_Direct3D->PIXEndEvent();

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
    auto device = m_Direct3D->GetD3DDevice();

	m_world = SimpleMath::Matrix::Identity;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(2.f, 2.f, 2.f),
	                              SimpleMath::Vector3::Zero, SimpleMath::Vector3::UnitY);
	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(m_Direct3D->GetScreenViewport().Width) / float(m_Direct3D->GetScreenViewport().Height), 0.1f, 10.f);

	m_LightPos = Vector4(0, 4, 0, 1);
}

void Game::OnDeviceLost()
{

}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

void Game::CreateGameObjects()
{
	ID3D11DeviceContext1* context = m_Direct3D->GetD3DDeviceContext();
	ID3D11Device1* device = m_Direct3D->GetD3DDevice();

	m_mesh = std::make_unique<Mesh>(device, context);

	HRESULT result = D3DX11CompileEffectFromFile(L"SimpleShader2.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
	                                             D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &m_pEffect, nullptr);
	if (FAILED(result))
	{
		LOG("something went wrong when compiling effect");
	}

	// Obtain the technique
	m_pTechnique = m_pEffect->GetTechniqueByName("Render");
	
	// Obtain the variables
	m_pWorldVariable = m_pEffect->GetVariableByName("World")->AsMatrix();
	m_pViewVariable = m_pEffect->GetVariableByName("View")->AsMatrix();
	m_pProjectionVariable = m_pEffect->GetVariableByName("Projection")->AsMatrix();
	m_lightPosVariable = m_pEffect->GetVariableByName("LightPos")->AsVector();



	//m_shader = std::make_unique<SimpleShader>();
	//m_shader->Initialize(device, *m_mesh);
}
