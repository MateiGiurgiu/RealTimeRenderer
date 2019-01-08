#include "pch.h"
#include "Shader.h"
#include "Texture.h"
#include <sstream>	
#include <cstring>
#include <sstream>
#include <d3dcompiler.h>

Shader::Shader(const wchar_t* const filename, ID3D11Device1* const device)
{
	ID3DBlob* error;
	const HRESULT result = D3DX11CompileEffectFromFile(filename, nullptr, reinterpret_cast<ID3DInclude*>(static_cast<UINT_PTR>(1)), static_cast<UINT>(D3DCOMPILE_ENABLE_STRICTNESS), 0, device, &m_effect, &error);
	if (FAILED(result))
	{
		if (error)
		{
			std::ostringstream s;
			s << "Error when compiling shader from: " << filename << "[" << static_cast<char*>(error->GetBufferPointer()) << "]";
			Log::PrintError(s.str());
			error->Release();
			error = nullptr;
		}
		else
		{
			std::ostringstream s;
			s << "Couldn't find shader file: " << filename;
			Log::PrintError(s.str());
		}
	}
	else
	{

		m_technique = m_effect->GetTechniqueByName("Render");
	}
}

Shader::~Shader()
{
	if (m_effect)
	{
		try
		{
			m_effect->Release();
		}
		catch (...)
		{

		}
	}
}

HRESULT Shader::SetInputLayout(const D3D11_INPUT_ELEMENT_DESC* const inputDesc, UINT const inputDescCount, ID3D11Device1* const device)
{
	D3DX11_PASS_DESC PassDesc;
	HRESULT result = m_technique->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(result))
	{
		return result;
	}

	result = device->CreateInputLayout(inputDesc, inputDescCount, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_vertexInputLayout);

	if (FAILED(result))
	{
		return result;
	}

	return result;
}

void Shader::PrepareForDraw(ID3D11DeviceContext1* const context, const int passIndex)
{
	context->IASetInputLayout(m_vertexInputLayout);
	
	DX::ThrowIfFailed(m_technique->GetPassByIndex(passIndex)->Apply(0, context));
}

ID3DX11EffectPass* Shader::GetPass(int const passIndex) const
{
	if(IsValid())
	{
		return m_technique->GetPassByIndex(passIndex);
	}
	else
	{
		return nullptr;
	}
}

void Shader::SetTexture(LPCSTR const varName, const Texture& texture)
{
	if (IsValid())
	{
		const auto var = m_effect->GetVariableByName(varName)->AsShaderResource();
		if (var->IsValid())
		{
			var->SetResource(texture.GetShaderResourceView());
		}
	}
}

void Shader::SetTexture(LPCSTR const varName, ID3D11ShaderResourceView* const texture)
{
	if (IsValid())
	{
		const auto var = m_effect->GetVariableByName(varName)->AsShaderResource();
		if (var->IsValid())
		{
			var->SetResource(texture);
		}
	}
}

void Shader::SetVector(LPCSTR const varName, DirectX::SimpleMath::Vector2 const & const vector)
{
	if (IsValid())
	{
		m_effect->GetVariableByName(varName)->AsVector()->SetFloatVector(reinterpret_cast<const float*>(&vector));
	}
}

void Shader::SetVector(LPCSTR const varName, DirectX::SimpleMath::Vector3 const & const vector)
{
	if (IsValid())
	{
		m_effect->GetVariableByName(varName)->AsVector()->SetFloatVector(reinterpret_cast<const float*>(&vector));
	}
}

void Shader::SetVector(LPCSTR const varName, DirectX::SimpleMath::Vector4 const & const vector)
{
	if (IsValid())
	{
		const auto var = m_effect->GetVariableByName(varName)->AsVector();
		if(var->IsValid())
		{
			var->SetFloatVector(reinterpret_cast<const float*>(&vector));
		}
	}
}

void Shader::SetColor(LPCSTR const varName, DirectX::SimpleMath::Color const & const color)
{
	if (IsValid())
	{
		const auto var = m_effect->GetVariableByName(varName)->AsVector();
		if (var->IsValid())
		{
			var->SetFloatVector(reinterpret_cast<const float*>(&color));
		}
	}
}

void Shader::SetMatrix(LPCSTR const varName, DirectX::SimpleMath::Matrix const & const matrix)
{
	if (IsValid())
	{
		const auto var = m_effect->GetVariableByName(varName)->AsMatrix();
		if(var->IsValid())
		{
			var->SetMatrix(reinterpret_cast<const float*>(&matrix));
		}
	}
}
