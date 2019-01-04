#include "pch.h"
#include "RenderQuad.h"

using namespace DirectX;

RenderQuad::RenderQuad(ID3D11Device1* device, UINT screenWidth, UINT screenHeight)
	: m_indexCount(0)
{
	CreateGeometryData(device);

	m_projectionMatrix = SimpleMath::Matrix::CreateOrthographic(static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f, 1.0f);
}

RenderQuad::RenderQuad(ID3D11Device1* device, std::shared_ptr<Shader> shader, UINT screenWidth, UINT screenHeight)
	: m_shader(shader), m_indexCount(0), m_layoutDescCount(2)
{
	CreateGeometryData(device);

	m_shader->SetInputLayout(m_layoutDesc, m_layoutDescCount, device);

	m_projectionMatrix = SimpleMath::Matrix::CreateOrthographic(static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f, 1.0f);
}

void RenderQuad::CreateGeometryData(ID3D11Device1* device)
{
	m_layoutDesc[0] = { "SV_Position",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0 };
	m_layoutDesc[1] = { "TEXCOORD",		0,	DXGI_FORMAT_R32G32_FLOAT,		0,	16,	D3D11_INPUT_PER_VERTEX_DATA, 0 };
	m_layoutDescCount = 2;

	Vertex vertices[] =
	{
		{ XMFLOAT4(-1.0f,	-1.0f, 0.0f, 1.0),	XMFLOAT2(0.0f, 0.0f)},
		{ XMFLOAT4(-1.0f,	 1.0f, 0.0f, 1.0),	XMFLOAT2(0.0f, 1.0f)},
		{ XMFLOAT4( 1.0f,	 1.0f, 0.0f, 1.0),	XMFLOAT2(1.0f, 1.0f)},
		{ XMFLOAT4( 1.0f,	-1.0f, 0.0f, 1.0),	XMFLOAT2(1.0f, 0.0f)},
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

void RenderQuad::PrepareForDraw(ID3D11DeviceContext1* context)
{
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderQuad::Draw(ID3D11DeviceContext1* context)
{
	if(m_shader)
	{
		PrepareForDraw(context);
		m_shader->SetMatrix("Projection", m_projectionMatrix);
		m_shader->PrepareForDraw(context);
		context->DrawIndexed(m_indexCount, 0, 0);
	}
}

void RenderQuad::DrawWithShader(ID3D11DeviceContext1* context, ID3D11Device1* device, Shader& shader)
{
	PrepareForDraw(context);

	DX::ThrowIfFailed(shader.SetInputLayout(m_layoutDesc, m_layoutDescCount, device));

	shader.SetMatrix("Projection", m_projectionMatrix);
	shader.PrepareForDraw(context);

	context->DrawIndexed(m_indexCount, 0, 0);
}



