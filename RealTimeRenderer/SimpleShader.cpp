#include "pch.h"
#include "SimpleShader.h"
#include "d3dcompiler.h"
#include "ReadData.h"

SimpleShader::SimpleShader()
{
}


SimpleShader::~SimpleShader()
{
}

bool SimpleShader::Initialize(ID3D11Device1* device, Mesh& mesh)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	/*
	result = D3DCompileFromFile(L"SimpleShader.fx", NULL, NULL, "VS", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage);
			LOG("Error compiling shader");
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			LOG("Shader file not found");
		}

		return false;
	}
	*/
	ShaderBuffer vertByte = LoadShader(L"../Debug/Test.cso");



	// Compile the pixel shader code.
	result = D3DCompileFromFile(L"SimpleShader.fx", NULL, NULL, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage);
			LOG("Error compiling shader");
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			LOG("Shader file not found");
		}

		return false;
	}


	// Create the vertex shader from the buffer.
	//result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	result = device->CreateVertexShader(vertByte.buffer, vertByte.size, nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}


	// Get a count of the elements in the layout.
	unsigned int numElements = mesh.GetLayoutDescCount();

	D3D11_INPUT_ELEMENT_DESC* desc = mesh.GetLayoutDesc();

	// Create the vertex input layout.
	//result = device->CreateInputLayout(desc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	result = device->CreateInputLayout(desc, numElements, vertByte.buffer, vertByte.size, &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	//vertexShaderBuffer->Release();
	//vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

std::string SimpleShader::GetName()
{
	return "Simple";
}

void SimpleShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage)
{
	char* compileErrors;
	unsigned long long bufferSize, i;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	LOG(compileErrors);


	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;
}

bool SimpleShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, Matrix worldMatrix, Matrix viewMatrix,
	Matrix projectionMatrix, Vector4 lightPos)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;


	// Transpose the matrices to prepare them for the shader.
	//worldMatrix = XMMatrixTranspose(worldMatrix);
	//viewMatrix = XMMatrixTranspose(viewMatrix);
	//projectionMatrix = XMMatrixTranspose(projectionMatrix);

	/*
	// Lock the constant buffer so it can be written to.
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->lightPos = lightPos;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	*/

	MatrixBufferType cb;
	cb.world = worldMatrix.Transpose();
	cb.view = viewMatrix.Transpose();
	cb.projection = projectionMatrix.Transpose();
	cb.lightPos = lightPos;
	deviceContext->UpdateSubresource(m_matrixBuffer, 0, nullptr, &cb, 0, 0);

	// Set the position of the constant buffer in the vertex shader.
	unsigned int bufferNumber = 0;
	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void SimpleShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
