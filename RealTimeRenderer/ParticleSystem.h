#pragma once
#include "SceneNode.h"
#include "Shader.h"

class ParticleSystem : public SceneNode
{
public:
	ParticleSystem(ID3D11Device1* device);
	~ParticleSystem();

	void Update(float deltaTime, float currentTime) override;
	void RenderForward(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;

	void PrepareForDraw(ID3D11DeviceContext1* context);
	void SetTexture(std::shared_ptr<Texture> texture) { m_texture = texture; }

private:
	void CreateGeometryData(ID3D11Device1* device);

	struct Vertex
	{
		DirectX::XMFLOAT4 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexCoord;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	std::shared_ptr<Shader> m_shader;

	unsigned int m_layoutDescCount;
	D3D11_INPUT_ELEMENT_DESC m_layoutDesc[3];

	UINT m_offset;
	UINT m_stride;
	UINT m_indexCount;

	unsigned int m_numOfParticles;
	DirectX::SimpleMath::Vector4* m_particleData;

	float m_currentTime;


	std::shared_ptr<Texture> m_texture;
};

