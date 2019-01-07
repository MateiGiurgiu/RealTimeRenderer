#pragma once
#include "SceneNode.h"
#include "MeshRenderer.h"

class VoxelTerrain : public SceneNode
{
public:
	explicit VoxelTerrain(ID3D11Device1* device);
	~VoxelTerrain();

	VoxelTerrain(const VoxelTerrain& other) = default;
	VoxelTerrain& operator=(const VoxelTerrain& other) = default;

	void Update(float deltaTime, float currentTime) override;
	void RenderDeferred(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;
	void RenderShadow(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) override;

	void RemoveAt(int x, int y, int z);
	void RemoveAtWithRadius(int x, int y, int z, float radius);

private:
	const int X_MAX = 25;
	const int Y_MAX = 10;
	const int Z_MAX = 15;

	std::unique_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Texture> m_diffuse;

	int m_numOfVoxels;

	DirectX::SimpleMath::Vector4* m_terrainData;
};

