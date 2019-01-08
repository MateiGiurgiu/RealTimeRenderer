#include "pch.h"
#include "ParticleSystem.h"
#include "ResourceManager.h"

using namespace DirectX;
using namespace SimpleMath;

ParticleSystem::ParticleSystem(ID3D11Device1* const device, const int numOfParticles, std::shared_ptr<Shader> shader)
	: m_shader(shader), m_numOfParticles(numOfParticles),  m_particleData(nullptr), m_currentTime(0.0f), m_lifeSpan(3.0f), m_doEmit(false)
{
	CreateGeometryData(device);
	m_shader->SetInputLayout(m_layoutDesc, m_layoutDescCount, device);
}


ParticleSystem::~ParticleSystem()
{
	if(m_particleData)
	{
		delete[] m_particleData;
		m_particleData = nullptr;
	}
}

void ParticleSystem::Update(float const deltaTime, float const currentTime)
{
	m_currentTime += deltaTime;
}

void ParticleSystem::RenderForward(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	if (!m_doEmit) return;

	if (m_texture)
	{
		m_shader->SetTexture("diffuseTex", *m_texture);
	}

	m_shader->GetEffect()->GetVariableByName("World")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&GetWorldMatrix()));
	m_shader->GetEffect()->GetVariableByName("View")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&view));
	m_shader->GetEffect()->GetVariableByName("Projection")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&proj));

	m_shader->GetEffect()->GetVariableByName("Time")->AsScalar()->SetFloat(m_currentTime);

	PrepareForDraw(context);
	m_shader->PrepareForDraw(context);
	context->DrawIndexedInstanced(m_indexCount, m_numOfParticles, 0, 0, 0);

	m_shader->SetTexture("diffuseTex", nullptr);
}

void ParticleSystem::PrepareForDraw(ID3D11DeviceContext1* const context)
{
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleSystem::CreateGeometryData(ID3D11Device1* const device)
{
	m_layoutDesc[0] = { "SV_Position",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0 };
	m_layoutDesc[1] = { "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	16,	D3D11_INPUT_PER_VERTEX_DATA, 0 };
	m_layoutDesc[2] = { "TEXCOORD",		0,	DXGI_FORMAT_R32G32_FLOAT,		0,	28,	D3D11_INPUT_PER_VERTEX_DATA, 0 };
	m_layoutDescCount = 3;

	Vertex vertices[] =
	{
		{ XMFLOAT4(-0.1f,	-0.1f, 0.0f, 1.0), XMFLOAT3(0,0,1),	XMFLOAT2(0.0f, 0.0f)},
		{ XMFLOAT4(-0.1f,	 0.1f, 0.0f, 1.0), XMFLOAT3(0,0,1),	XMFLOAT2(0.0f, 1.0f)},
		{ XMFLOAT4( 0.1f,	 0.1f, 0.0f, 1.0), XMFLOAT3(0,0,1),	XMFLOAT2(1.0f, 1.0f)},
		{ XMFLOAT4( 0.1f,	-0.1f, 0.0f, 1.0), XMFLOAT3(0,0,1),	XMFLOAT2(1.0f, 0.0f)},
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * 4;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, &initData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	WORD indices[] =
	{
		3,1,0,
		2,1,3,
	};

	m_indexCount = 6;
	m_offset = 0;
	m_stride = sizeof(Vertex);

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(WORD) * 6;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	initData.pSysMem = indices;
	DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, &initData, m_indexBuffer.ReleaseAndGetAddressOf()));
}
