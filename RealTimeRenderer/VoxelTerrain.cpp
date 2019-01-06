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

void VoxelTerrain::Update(float deltaTime)
{
}

void VoxelTerrain::Render(ID3D11DeviceContext1* context, Matrix view, Matrix proj)
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

void VoxelTerrain::RenderShadow(ID3D11DeviceContext1* context, Matrix view, Matrix proj)
{
	m_meshRenderer->SetInstanceData(m_terrainData, sizeof(Vector4), m_numOfVoxels);
	m_meshRenderer->DrawShadowInstanced(context, GetWorldMatrix(), view, proj, m_numOfVoxels);
}

void VoxelTerrain::RemoveAt(int x, int y, int z)
{
	m_terrainData[x + y * X_MAX + z * (X_MAX * Y_MAX)].w = 0;
}

void VoxelTerrain::RemoveAtWithRadius(int x, int y, int z, float radius)
{
	Vector3 centre = Vector3(x, y, z);
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
