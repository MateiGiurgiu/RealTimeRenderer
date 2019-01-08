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
	void RenderDeferred(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;
	void RenderShadow(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

	void RemoveAt(int x, int y, int z);
	void RemoveAtWithRadius(int x, int y, int z, float radius);
	bool SpaceshipHit(DirectX::SimpleMath::Vector3 const & position);

	void Reset();

private:
	const int X_MAX = 30;
	const int Y_MAX = 10;
	const int Z_MAX = 12;

	unsigned int Clamp(unsigned int value, unsigned int min, unsigned int max) const;
	bool InsideBounds(DirectX::SimpleMath::Vector3 const & position) const;

	std::unique_ptr<MeshRenderer> m_meshRenderer;
	std::shared_ptr<Texture> m_diffuse;

	int m_numOfVoxels;

	DirectX::SimpleMath::Vector4* m_terrainData;
};

