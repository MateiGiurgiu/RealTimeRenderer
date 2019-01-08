#include "pch.h"
#include "VoxelTerrain.h"
#include "ResourceManager.h"

using namespace DirectX::SimpleMath;

VoxelTerrain::VoxelTerrain(ID3D11Device1* device)
	: m_terrainData(nullptr)
{
	m_meshRenderer = std::make_unique<MeshRenderer>
	(
		ResourceManager::GetMesh(L"Models/Voxel.sdkmesh", device),
		ResourceManager::GetShader(L"Shaders/VoxelTerrain.fx", device),
		device
	);

	m_meshRenderer->SetShadowShader(device, ResourceManager::GetShader(L"Shaders/VoxelTerrainShadow.fx", device));

	m_diffuse = ResourceManager::GetTexture(L"Textures/VoxelTile.png", device);

	m_numOfVoxels = X_MAX * Y_MAX * Z_MAX;
	m_terrainData = new Vector4[m_numOfVoxels];

	// set up the voxel layout
	for(int y = 0; y < Y_MAX; y++)
	{
		for (int x = 0; x < X_MAX; x++)
		{
			for (int z = 0; z < Z_MAX; z++)
			{
				//data[x][y][z] = data[x + ymaxX + zmaxX*maxY]
				m_terrainData[x + y * X_MAX + z * (X_MAX * Y_MAX)] = Vector4
				(
					(x * 0.5) - static_cast<float>(X_MAX) * 0.25,
					(-y * 0.5) - 0.5f,
					(z * 0.5) - static_cast<float>(Z_MAX) * 0.25,
					1.0
				);
			}
		}
	}
}



VoxelTerrain::~VoxelTerrain()
{
	if(m_terrainData)
	{
		delete[] m_terrainData;
		m_terrainData = nullptr;
	}
}

void VoxelTerrain::Update(float const deltaTime, float const currentTime)
{
}

void VoxelTerrain::RenderDeferred(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	if (m_diffuse)
	{
		m_meshRenderer->GetShader()->SetTexture("diffuseTex", *m_diffuse);
	}
	else
	{
		m_meshRenderer->GetShader()->SetTexture("diffuseTex", nullptr);
	}

	m_meshRenderer->SetInstanceData(m_terrainData, sizeof(Vector4), m_numOfVoxels);
	m_meshRenderer->DrawInstanced(context, GetWorldMatrix(), view, proj, m_numOfVoxels);
}

void VoxelTerrain::RenderShadow(ID3D11DeviceContext1* const context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	m_meshRenderer->SetInstanceData(m_terrainData, sizeof(Vector4), m_numOfVoxels);
	m_meshRenderer->DrawShadowInstanced(context, GetWorldMatrix(), view, proj, m_numOfVoxels);
}

void VoxelTerrain::RemoveAt(int const x, int const y, int const z)
{
	m_terrainData[x + y * X_MAX + z * (X_MAX * Y_MAX)].w = 0;
}

void VoxelTerrain::RemoveAtWithRadius(int const x, int const y, int const z, float const radius)
{
	const Vector3 centre = Vector3(x, y, z);
	for(int i = 0; i< m_numOfVoxels; i++)
	{
		Vector3 voxelPos = Vector3(m_terrainData[i].x, m_terrainData[i].y, m_terrainData[i].z);
		voxelPos += Vector3(0.25, 0.25, 0.25);
		if((centre - voxelPos).Length() < radius)
		{
			m_terrainData[i].w = 0.0f;
		}
	}
}

bool VoxelTerrain::SpaceshipHit(Vector3 position)
{
	if (position.y < 0.0f)
	{
		Vector3 relativePos = Vector3(
			(position.x + static_cast<float>(X_MAX) * 0.25f) * 2,
			(-position.y + 0.5f) * 2,
			(position.z + static_cast<float>(Z_MAX) * 0.25) * 2
		);
		unsigned int x = Clamp(static_cast<unsigned int>(relativePos.x), 0, X_MAX);
		unsigned int y = Clamp(static_cast<unsigned int>(relativePos.y), 0, Y_MAX);
		unsigned int z = Clamp(static_cast<unsigned int>(relativePos.z), 0, Z_MAX);

		if (InsideBounds(relativePos))
		{
			if (m_terrainData[x + y * X_MAX + z * (X_MAX * Y_MAX)].w < 0.1)
			{
				return false;
			}
			else
			{
				//RemoveAtWithRadius(position.x, position.y, position.z, 0.9f);
				m_terrainData[x + y * X_MAX + z * (X_MAX * Y_MAX)].w = 0.0f;
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void VoxelTerrain::Reset()
{
	for (unsigned int i = 0; i < m_numOfVoxels; i++)
	{
		m_terrainData[i].w = 1;
	}
}

unsigned int VoxelTerrain::Clamp(unsigned int value, unsigned int min, unsigned int max)
{
	if (value > max) value = max;
	if (value < min) value = min;
	return value;
}

bool VoxelTerrain::InsideBounds(DirectX::SimpleMath::Vector3 position)
{
	unsigned int x = static_cast<unsigned int>(position.x);
	unsigned int y = static_cast<unsigned int>(position.y);
	unsigned int z = static_cast<unsigned int>(position.z);

	bool bX = (x >= 0 && x < X_MAX);
	bool bY = (y >= 0 && y < Y_MAX);
	bool bZ = (z >= 0 && z < Z_MAX);

	return bX && bY && bZ;
}
