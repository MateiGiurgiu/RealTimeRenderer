#include "pch.h"
#include "Shader.h"
#include "Texture.h"
#include <sstream>	
#include <cstring>
#include <sstream>
#include <d3dcompiler.h>

Shader::Shader(const wchar_t* filename, ID3D11Device1* device)
{
	ID3DBlob* error;
	HRESULT result = D3DX11CompileEffectFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &m_effect, &error);
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

HRESULT Shader::SetInputLayout(const D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT inputDescCount, ID3D11Device1* device)
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

void Shader::PrepareForDraw(ID3D11DeviceContext1* context, const int passIndex)
{
	context->IASetInputLayout(m_vertexInputLayout);
	
	DX::ThrowIfFailed(m_technique->GetPassByIndex(passIndex)->Apply(0, context));
}

ID3DX11EffectPass* Shader::GetPass(int passIndex) const
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

void Shader::SetTexture(LPCSTR varName, const Texture& texture)
{
	if (IsValid())
	{
		auto var = m_effect->GetVariableByName(varName)->AsShaderResource();
		if (var->IsValid())
		{
			var->SetResource(texture.GetShaderResourceView());
		}
	}
}

void Shader::SetTexture(LPCSTR varName, ID3D11ShaderResourceView* texture)
{
	if (IsValid())
	{
		auto var = m_effect->GetVariableByName(varName)->AsShaderResource();
		if (var->IsValid())
		{
			var->SetResource(texture);
		}
	}
}

void Shader::SetVector(LPCSTR varName, DirectX::SimpleMath::Vector2& vector)
{
	if (IsValid())
	{
		m_effect->GetVariableByName(varName)->AsVector()->SetFloatVector(reinterpret_cast<float*>(&vector));
	}
}

void Shader::SetVector(LPCSTR varName, DirectX::SimpleMath::Vector3& vector)
{
	if (IsValid())
	{
		m_effect->GetVariableByName(varName)->AsVector()->SetFloatVector(reinterpret_cast<float*>(&vector));
	}
}

void Shader::SetVector(LPCSTR varName, DirectX::SimpleMath::Vector4& vector)
{
	if (IsValid())
	{
		auto var = m_effect->GetVariableByName(varName)->AsVector();
		if(var->IsValid())
		{
			var->SetFloatVector(reinterpret_cast<float*>(&vector));
		}
	}
}

void Shader::SetColor(LPCSTR varName, DirectX::SimpleMath::Color& color)
{
	if (IsValid())
	{
		auto var = m_effect->GetVariableByName(varName)->AsVector();
		if (var->IsValid())
		{
			var->SetFloatVector(reinterpret_cast<float*>(&color));
		}
	}
}

void Shader::SetMatrix(LPCSTR varName, DirectX::SimpleMath::Matrix& matrix)
{
	if (IsValid())
	{
		auto var = m_effect->GetVariableByName(varName)->AsMatrix();
		if(var->IsValid())
		{
			var->SetMatrix(reinterpret_cast<float*>(&matrix));
		}
	}
}
