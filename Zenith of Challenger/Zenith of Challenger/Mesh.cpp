//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "mesh.h"

void MeshBase::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->DrawInstanced(m_vertices, 1, 0, 0);
}

void MeshBase::ReleaseUploadBuffer()
{
	if (m_vertexUploadBuffer) m_vertexUploadBuffer.Reset();
}

TerrainMesh::TerrainMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	LoadMesh(device, commandList, fileName);
}

void TerrainMesh::LoadMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
    // Open the .raw file
    ifstream in(fileName, ios::binary);
    if (!in.is_open()) {
        throw runtime_error("Failed to open heightmap file.");
    }

    // Determine the dimensions of the heightmap
    size_t size;
    in.seekg(0, ios::end);
    size = static_cast<size_t>(in.tellg());
    in.seekg(0, ios::beg);

    int length = static_cast<int>(sqrt(size)); // Assuming square dimensions
    if (length * length != size) {
        throw runtime_error("Heightmap file is not a perfect square.");
    }

    int middle = length / 2;

    // Resize the height vector
    m_height.resize(length, vector<BYTE>(length));

    // Read the heightmap data
    for (int z = 0; z < length; ++z) {
        in.read(reinterpret_cast<char*>(m_height[z].data()), length * sizeof(BYTE));
    }

    // Close the file
    in.close();

    // Vertex creation
    const float delta = 1.f / static_cast<float>(length);
    vector<DetailVertex> vertices;

    for (int z = 0; z < length - 1; ++z) {
        for (int x = 0; x < length - 1; ++x) {
            float nx = static_cast<float>(x - middle);
            float nz = static_cast<float>(z - middle);
            float dx = static_cast<float>(x) * delta;
            float dz = 1.f - static_cast<float>(z) * delta;

            vertices.emplace_back(XMFLOAT3{ nx, static_cast<float>(m_height[z][x]), nz },
                XMFLOAT2{ dx, dz }, XMFLOAT2{ 0.f, 1.f });
            vertices.emplace_back(XMFLOAT3{ nx, static_cast<float>(m_height[z + 1][x]), nz + 1 },
                XMFLOAT2{ dx, dz - delta }, XMFLOAT2{ 0.f, 0.f });
            vertices.emplace_back(XMFLOAT3{ nx + 1, static_cast<float>(m_height[z + 1][x + 1]), nz + 1 },
                XMFLOAT2{ dx + delta, dz - delta }, XMFLOAT2{ 1.f, 0.f });

            vertices.emplace_back(XMFLOAT3{ nx, static_cast<float>(m_height[z][x]), nz },
                XMFLOAT2{ dx, dz }, XMFLOAT2{ 0.f, 1.f });
            vertices.emplace_back(XMFLOAT3{ nx + 1, static_cast<float>(m_height[z + 1][x + 1]), nz + 1 },
                XMFLOAT2{ dx + delta, dz - delta }, XMFLOAT2{ 1.f, 0.f });
            vertices.emplace_back(XMFLOAT3{ nx + 1, static_cast<float>(m_height[z][x + 1]), nz },
                XMFLOAT2{ dx + delta, dz }, XMFLOAT2{ 1.f, 1.f });
        }
    }

    // Create the vertex buffer
    CreateVertexBuffer(device, commandList, vertices);
}

