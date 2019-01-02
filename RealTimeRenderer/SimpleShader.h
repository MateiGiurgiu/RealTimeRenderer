#pragma once
#include "IShader.h"
#include "ReadData.h"
#include "Mesh.h"


class SimpleShader : public IShader
{

public:
	SimpleShader();
	~SimpleShader();

	bool Initialize(ID3D11Device1* device, Mesh& mesh) override;
	std::string GetName() override;
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage) override;
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix, Vector4 lightPos) override;
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) override;

private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
		Vector4 lightPos;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};

