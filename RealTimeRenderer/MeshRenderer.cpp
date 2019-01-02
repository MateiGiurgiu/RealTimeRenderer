#include "pch.h"
#include "MeshRenderer.h"

using namespace DirectX::SimpleMath;

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Shader>& shader, ID3D11Device1* device)
	: m_mesh(mesh), m_shader(shader)
{
	HRESULT result = m_shader->SetInputLayout(m_mesh->GetLayoutDesc(), m_mesh->GetLayoutDescCount(), device);
	if (FAILED(result))
	{
		LOG_ERROR("Error when setting shader layout");
	}

	m_pWorldVariable = m_shader->GetEffect()->GetVariableByName("World")->AsMatrix();
	m_pViewVariable = m_shader->GetEffect()->GetVariableByName("View")->AsMatrix();
	m_pProjVariable = m_shader->GetEffect()->GetVariableByName("Projection")->AsMatrix();
}

void MeshRenderer::Render(ID3D11DeviceContext1* context, Matrix world, Matrix view, Matrix proj)
{
	m_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&world));
	m_pViewVariable->SetMatrix(reinterpret_cast<float*>(&view));
	m_pProjVariable->SetMatrix(reinterpret_cast<float*>(&proj));

	m_mesh->PrepareForDraw(context);
	m_shader->PrepareForDraw(context);

	context->DrawIndexed(m_mesh->GetIndexCount(), 0, 0);
}

