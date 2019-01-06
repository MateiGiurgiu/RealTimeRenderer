#pragma once
#include "SceneNode.h"
#include "MeshRenderer.h"

class VoxelTerrain : public SceneNode
{
public:
	const int X_MAX = 40;
	const int Y_MAX = 10;
	const int Z_MAX = 10;

	VoxelTerrain(ID3D11Device1* device);
	~VoxelTerrain();

	void Update(float deltaTime) override;
	void Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;
	void RenderShadow(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;

	void RemoveAt(int x, int y, int z);
	void RemoveAtWithRadius(int x, int y, int z, float radius);

private:
	std::unique_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Texture> m_diffuse;

	int m_numOfVoxels;

	DirectX::SimpleMath::Vector4* m_terrainData;
};

