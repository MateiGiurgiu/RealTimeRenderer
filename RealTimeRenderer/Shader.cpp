#include "pch.h"
#include "Shader.h"
#include <d3dcompiler.h>

Shader::Shader(wchar_t* filename, ID3D11Device1* device)
{
	ID3DBlob* error;
	HRESULT result = D3DX11CompileEffectFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &m_effect, &error);
	if (FAILED(result))
	{
		if (error)
		{
			LOG_ERROR("Error when compiling shader from: " << filename << "[" << static_cast<char*>(error->GetBufferPointer()) << "]");
			error->Release();
			error = nullptr;
		}
		else
		{
			LOG_ERROR("Couldn't find shader file: " << filename);
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
		m_effect->Release();
	}
}

HRESULT Shader::SetInputLayout(D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT inputDescCount, ID3D11Device1* device)
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
	m_technique->GetPassByIndex(passIndex)->Apply(0, context);
}
