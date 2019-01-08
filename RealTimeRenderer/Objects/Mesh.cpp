#include "pch.h"
#include "Mesh.h"
#include "Effects.h"
#include "Model.h"

Mesh::Mesh(const wchar_t* const filename, ID3D11Device1* device) :
	m_vertexBuffer(nullptr), 
	m_indexBuffer(nullptr)
{
	std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
	const std::unique_ptr<DirectX::Model> model = DirectX::Model::CreateFromSDKMESH(device, filename, *fx);

	model->meshes[0]->meshParts[0]->vertexBuffer.Swap(m_vertexBuffer);
	model->meshes[0]->meshParts[0]->indexBuffer.Swap(m_indexBuffer);

	m_indexCount = model->meshes[0]->meshParts[0]->indexCount;
	D3D11_BUFFER_DESC desc;
	m_vertexBuffer->GetDesc(&desc);
	
	m_vertexOffset = model->meshes[0]->meshParts[0]->vertexOffset;
	m_vertexStride = model->meshes[0]->meshParts[0]->vertexStride;

	std::vector<D3D11_INPUT_ELEMENT_DESC>* const layoutDesc = model->meshes[0]->meshParts[0]->vbDecl.get();

	m_layoutDescCount = layoutDesc->size();
	m_layoutDesc = new D3D11_INPUT_ELEMENT_DESC[m_layoutDescCount];

	for(int i = 0; i < m_layoutDescCount; ++i)
	{
		m_layoutDesc[i] = layoutDesc->at(i);
	}
}

Mesh::~Mesh()
{
	if (m_indexBuffer)
	{
		try
		{
			m_indexBuffer.Reset();
		}
		catch (...)
		{

		}
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		try
		{
			m_vertexBuffer.Reset();
		}
		catch (...)
		{

		}
	}

	if (m_layoutDesc)
	{
		delete[] m_layoutDesc;
	}
}

void Mesh::PrepareForDraw(ID3D11DeviceContext1* const context)
{
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_vertexStride, &m_vertexOffset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
