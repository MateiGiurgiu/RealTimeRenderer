#pragma once
#include "SceneNode.h"
#include "Shader.h"

class ParticleSystem : public SceneNode
{
public:
	ParticleSystem(ID3D11Device1* device, int numOfParticles, std::shared_ptr<Shader> shader);
	~ParticleSystem();

	ParticleSystem(const ParticleSystem& other) = default;
	ParticleSystem& operator=(const ParticleSystem& other) = default;

	void Update(float deltaTime, float currentTime) override;
	void RenderForward(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

	void PrepareForDraw(ID3D11DeviceContext1* context);
	void SetTexture(std::shared_ptr<Texture> const texture) { m_texture = texture; }
	void SetEmit(bool const value) { m_doEmit = value; }
	void SetTime(float const time) { m_currentTime = time; }
	void ResetTime() { m_currentTime = 0.0f; }
	bool IsEmitting() const { return m_doEmit; }

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
	float m_lifeSpan;
	bool m_doEmit;

	std::shared_ptr<Texture> m_texture;
};

