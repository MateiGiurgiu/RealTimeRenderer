#pragma once
#include "pch.h"
#include "Mesh.h"
using namespace DirectX::SimpleMath;

class IShader
{

public:
	virtual ~IShader() = default;

	IShader(const IShader&) = delete;
	IShader& operator=(const IShader&) = delete;

	IShader(IShader&&) = delete;
	IShader& operator=(IShader&&) = delete;

	virtual std::string GetName() = 0;
	virtual bool Initialize(ID3D11Device1* device, Mesh& mesh) = 0;
	virtual void OutputShaderErrorMessage(ID3D10Blob* errorMessage) = 0;
	virtual bool SetShaderParameters(ID3D11DeviceContext* deviceContext, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix, Vector4 lightPos) = 0;
	virtual void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) = 0;

protected:
	IShader() = default;
};

