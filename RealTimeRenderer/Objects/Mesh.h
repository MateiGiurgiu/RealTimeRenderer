#pragma once
#include "pch.h"

class Mesh
{
public:
	Mesh(const wchar_t* filename, ID3D11Device1* device);
	~Mesh();

	Mesh(const Mesh& other) = default;
	Mesh& operator=(const Mesh& other) = default;



	void PrepareForDraw(ID3D11DeviceContext1* context);

	// Accessors
	unsigned int GetIndexCount() const { return m_indexCount; };
	unsigned int GetLayoutDescCount() const { return m_layoutDescCount; }
	const D3D11_INPUT_ELEMENT_DESC* GetLayoutDesc() { return m_layoutDesc; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	unsigned int m_indexCount;

	unsigned int m_vertexOffset;
	unsigned int m_vertexStride;

	unsigned int m_layoutDescCount;
	D3D11_INPUT_ELEMENT_DESC* m_layoutDesc;
};

