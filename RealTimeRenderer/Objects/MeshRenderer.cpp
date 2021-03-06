#include "pch.h"
#include "MeshRenderer.h"

using namespace DirectX::SimpleMath;

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Shader>& shader, ID3D11Device1* const device)
	: m_mesh(mesh), m_shader(shader)
{
	DX::ThrowIfFailed(m_shader->SetInputLayout(m_mesh->GetLayoutDesc(), m_mesh->GetLayoutDescCount(), device));

	m_pWorldVariable = m_shader->GetEffect()->GetVariableByName("World")->AsMatrix();
	m_pViewVariable = m_shader->GetEffect()->GetVariableByName("View")->AsMatrix();
	m_pProjVariable = m_shader->GetEffect()->GetVariableByName("Projection")->AsMatrix();
}

void MeshRenderer::PrepareForDraw(ID3D11DeviceContext1* const context, const Matrix& world, const Matrix& view, const Matrix& proj) const
{
	m_pWorldVariable->SetMatrix(reinterpret_cast<const float*>(&world));
	m_pViewVariable->SetMatrix(reinterpret_cast<const float*>(&view));
	m_pProjVariable->SetMatrix(reinterpret_cast<const float*>(&proj));

	m_mesh->PrepareForDraw(context);
	m_shader->PrepareForDraw(context);
}

void MeshRenderer::Draw(ID3D11DeviceContext1* const context, const Matrix& world, const Matrix& view, const Matrix& proj) const
{
	PrepareForDraw(context, world, view, proj);

	context->DrawIndexed(m_mesh->GetIndexCount(), 0, 0);
}

void MeshRenderer::Draw(ID3D11DeviceContext1* const context, const Matrix& world, const Matrix& view, const Matrix& proj, ID3D11RasterizerState* const rState) const
{
	PrepareForDraw(context, world, view, proj);

	context->RSSetState(rState);

	context->DrawIndexed(m_mesh->GetIndexCount(), 0, 0);
}

void MeshRenderer::DrawShadow(ID3D11DeviceContext1* const context, const Matrix& world, const Matrix& view, const Matrix& proj) const
{
	if(m_shadowShader)
	{
		m_shadowShader->GetEffect()->GetVariableByName("World")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&world));
		m_shadowShader->GetEffect()->GetVariableByName("View")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&view));
		m_shadowShader->GetEffect()->GetVariableByName("Projection")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&proj));

		m_mesh->PrepareForDraw(context);
		m_shadowShader->PrepareForDraw(context);
		context->DrawIndexed(m_mesh->GetIndexCount(), 0, 0);
	}
}

void MeshRenderer::SetInstanceData(void* const data, const UINT dataSize, const UINT numOfInstances) const
{
	m_shader->GetEffect()->GetVariableByName("InstanceData")->AsVector()->SetRawValue(data, 0, dataSize * numOfInstances);
	if (m_shadowShader)
	{
		const auto var = m_shadowShader->GetEffect()->GetVariableByName("InstanceData")->AsVector();
		if (var->IsValid())
		{
			var->SetRawValue(data, 0, dataSize * numOfInstances);
		}
	}
}

void MeshRenderer::SetInstanceData(const LPCSTR instanceVarName, void* const data, const UINT dataSize, const UINT numOfInstances) const
{
	m_shader->GetEffect()->GetVariableByName(instanceVarName)->AsVector()->SetRawValue(data, 0, dataSize * numOfInstances);
	if (m_shadowShader)
	{
		const auto var = m_shadowShader->GetEffect()->GetVariableByName(instanceVarName)->AsVector();
		if (var->IsValid())
		{
			var->SetRawValue(data, 0, dataSize * numOfInstances);
		}
	}
}

void MeshRenderer::DrawInstanced(ID3D11DeviceContext1* const context, const Matrix& world, const Matrix& view, const Matrix& proj, const UINT numOfInstances) const
{
	PrepareForDraw(context, world, view, proj);

	context->DrawIndexedInstanced(m_mesh->GetIndexCount(), numOfInstances, 0, 0, 0);
}

void MeshRenderer::DrawShadowInstanced(ID3D11DeviceContext1* const context, const Matrix& world, const Matrix& view, const Matrix& proj, const UINT numOfInstances) const
{
	if (m_shadowShader)
	{
		m_shadowShader->GetEffect()->GetVariableByName("World")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&world));
		m_shadowShader->GetEffect()->GetVariableByName("View")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&view));
		m_shadowShader->GetEffect()->GetVariableByName("Projection")->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(&proj));

		m_mesh->PrepareForDraw(context);
		m_shadowShader->PrepareForDraw(context);
		context->DrawIndexedInstanced(m_mesh->GetIndexCount(), numOfInstances, 0, 0, 0);
	}
}

void MeshRenderer::SetShadowShader(ID3D11Device1* const device, const std::shared_ptr<Shader> shadowShader)
{
	m_shadowShader = shadowShader;
	const HRESULT result = m_shadowShader->SetInputLayout(m_mesh->GetLayoutDesc(), m_mesh->GetLayoutDescCount(), device);

	if (FAILED(result))
	{
		Log::PrintError("Error when setting shader layout");
	}
}
